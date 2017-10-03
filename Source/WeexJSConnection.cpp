#include "WeexJSConnection.h"
#include "IPCException.h"
#include "IPCLog.h"
#include "IPCSender.h"
#include "LogUtils.h"
#include "Trace.h"
#include "ashmem.h"
#include "IPCFutexPageQueue.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <sys/mman.h>
#include <vector>
#include <iostream>
#include <fstream>

static void doExec(int fd, bool traceEnable);
static void closeAllButThis(int fd);
extern const char* s_cacheDir;
static std::string logFilePath = "/data/data/com.taobao.taobao/cache";
static void printLogOnFile(const char* log);

struct WeexJSConnection::WeexJSConnectionImpl {
    std::unique_ptr<IPCSender> serverSender;
    std::unique_ptr<IPCFutexPageQueue> futexPageQueue;
    pid_t child{ 0 };
};

WeexJSConnection::WeexJSConnection()
    : m_impl(new WeexJSConnectionImpl)
{
}

WeexJSConnection::~WeexJSConnection()
{
    end();
}

IPCSender* WeexJSConnection::start(IPCHandler* handler, bool reinit)
{
    int fd = ashmem_create_region("WEEX_IPC", IPCFutexPageQueue::ipc_size);
    if (-1 == fd) {
        throw IPCException("failed to create ashmem region: %s", strerror(errno));
    }
    void* base = mmap(nullptr, IPCFutexPageQueue::ipc_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (base == MAP_FAILED) {
        int _errno = errno;
        close(fd);
        throw IPCException("failed to map ashmem region: %s", strerror(_errno));
    }
    std::unique_ptr<IPCFutexPageQueue> futexPageQueue(new IPCFutexPageQueue(base, IPCFutexPageQueue::ipc_size, 0));
    std::unique_ptr<IPCSender> sender(createIPCSender(futexPageQueue.get(), handler));
    m_impl->serverSender = std::move(sender);
    m_impl->futexPageQueue = std::move(futexPageQueue);
#if PRINT_LOG_CACHEFILE
    if (s_cacheDir) {
        logFilePath = s_cacheDir;
    }
    logFilePath.append("/jsserver_start.log");
    std::ofstream mcfile;
    if (reinit) {
        mcfile.open(logFilePath, std::ios::app);
        mcfile << "restart fork a process" << std::endl;
    } else {
        mcfile.open(logFilePath);
        mcfile << "start fork a process" << std::endl;
    }
#endif
    pid_t child;
    if (reinit) {
#if PRINT_LOG_CACHEFILE
        mcfile << "reinit is ture use vfork" << std::endl;
        mcfile.close();
#endif
        child = vfork();
    } else {
#if PRINT_LOG_CACHEFILE
        mcfile << "reinit is false use fork" << std::endl;
        mcfile.close();
#endif
        child = fork();
    }
    if (child == -1) {
        int myerrno = errno;
        munmap(base, IPCFutexPageQueue::ipc_size);
        close(fd);
        throw IPCException("failed to fork: %s", strerror(myerrno));
    } else if (child == 0) {
        // the child
        closeAllButThis(fd);
        // implements close all but handles[1]
        // do exec
        printLogOnFile("fork success on subprocess and start doExec");;
        doExec(fd, base::debug::TraceEvent::isEnable());
        printLogOnFile("exec Failed completely.");
        LOGE("exec Failed completely.");
        // failed to exec
        _exit(1);
    } else {
        printLogOnFile("fork success on main process and start m_impl->futexPageQueue->spinWaitPeer()");

        close(fd);
        m_impl->child = child;
        m_impl->futexPageQueue->spinWaitPeer();
    }
    return m_impl->serverSender.get();
}

void WeexJSConnection::end()
{
    m_impl->serverSender.reset();
    m_impl->futexPageQueue.reset();
    if (m_impl->child) {
        int wstatus;
        pid_t child;
        kill(m_impl->child, 9);
        while (true) {
            child = waitpid(m_impl->child, &wstatus, 0);
            if (child != -1)
                break;
            if (errno != EINTR)
                break;
        }
    }
}

void printLogOnFile(const char* log) {
#if PRINT_LOG_CACHEFILE
    std::ofstream mcfile;
    mcfile.open(logFilePath, std::ios::app);
    mcfile << log << std::endl;
    mcfile.close();
#endif
}

static std::string __attribute__((noinline)) findPath();
static void findPath(std::string& executablePath, std::string& icuDataPath)
{
    unsigned long target = reinterpret_cast<unsigned long>(__builtin_return_address(0));
    FILE* f = fopen("/proc/self/maps", "r");
    if (!f) {
        return;
    }
    char buffer[256];
    char* line;
    while ((line = fgets(buffer, 256, f))) {
        if (icuDataPath.empty() && strstr(line, "icudt")) {
            icuDataPath.assign(strstr(line, "/"));
            icuDataPath = icuDataPath.substr(0, icuDataPath.length() - 1);
            continue;
        }
        char* end;
        unsigned long val;
        errno = 0;
        val = strtoul(line, &end, 16);
        if (errno)
            continue;
        if (val > target)
            continue;
        end += 1;
        errno = 0;
        val = strtoul(end, &end, 16);
        if (errno)
            continue;
        if (val > target) {
            executablePath.assign(strstr(end, "/"));
            std::size_t found = executablePath.rfind('/');
            if (found != std::string::npos) {
                executablePath = executablePath.substr(0, found);
            }
        }
        if (!executablePath.empty()
            && !icuDataPath.empty()) {
            break;
        }
    }
    fclose(f);
    return;
}

class EnvPBuilder {
public:
    EnvPBuilder();
    ~EnvPBuilder() = default;

    void addNew(const char* n);
    std::unique_ptr<const char* []> build();

private:
    std::vector<const char*> m_vec;
};

EnvPBuilder::EnvPBuilder()
{
    for (char** env = environ; *env; env++) {
        // fixme:add for ANDROID_ROOT envp
        // if cannot find some env, can use such as
        // PATH/ANDROID_BOOTLOGO/ANDROID_ASSETS/ANDROID_DATA/ASEC_MOUNTPOINT
        // LOOP_MOUNTPOINT/BOOTCLASSPATH and etc
        // but don't use LD_LIBRARY_PATH env may cause so cannot be found
        const char *android_root_env = "ANDROID_ROOT=";
        if (std::strstr(*env, android_root_env) != nullptr) { 
            addNew(*env);
            break;
        }
    }
}

void EnvPBuilder::addNew(const char* n)
{
    m_vec.emplace_back(n);
}

std::unique_ptr<const char* []> EnvPBuilder::build() {
    std::unique_ptr<const char* []> ptr(new const char*[m_vec.size() + 1]);
    for (size_t i = 0; i < m_vec.size(); ++i) {
        ptr.get()[i] = m_vec[i];
    }
    ptr.get()[m_vec.size()] = nullptr;
    return ptr;
}

void doExec(int fd, bool traceEnable)
{
    std::string executablePath;
    std::string icuDataPath;
    findPath(executablePath, icuDataPath);
#if PRINT_LOG_CACHEFILE
    std::ofstream mcfile;
    mcfile.open(logFilePath , std::ios::app);
    mcfile << "jsengine WeexJSConnection::doExec executablePath:" << executablePath << std::endl;
    mcfile << "jsengine WeexJSConnection::doExec icuDataPath:" << icuDataPath << std::endl;
#endif

    if (executablePath.empty()) {
        LOGE("executablePath is empty");

#if PRINT_LOG_CACHEFILE
        mcfile << "jsengine WeexJSConnection::doExec executablePath is empty and return" << std::endl;
        mcfile.close();
#endif

        return;
    }
    if (icuDataPath.empty()) {
        LOGE("icuDataPath is empty");
#if PRINT_LOG_CACHEFILE
        mcfile << "jsengine WeexJSConnection::doExec icuDataPath is empty and return" << std::endl;
        mcfile.close();
#endif
        return;
    }
    std::string ldLibraryPathEnv("LD_LIBRARY_PATH=");
    std::string icuDataPathEnv("ICU_DATA_PATH=");
    std::string crashFilePathEnv("CRASH_FILE_PATH=");
    ldLibraryPathEnv.append(executablePath);
    icuDataPathEnv.append(icuDataPath);
#if PRINT_LOG_CACHEFILE
    mcfile << "jsengine ldLibraryPathEnv:" << ldLibraryPathEnv << " icuDataPathEnv:" << icuDataPathEnv << std::endl;
#endif
    if (!s_cacheDir) {
        LOGE("crash log file path s_cacheDir is empty");
        crashFilePathEnv.append(logFilePath);
    } else {
        crashFilePathEnv.append(s_cacheDir);
    }
    crashFilePathEnv.append("/jsserver_crash");
    char fdStr[16];
    snprintf(fdStr, 16, "%d", fd);
    EnvPBuilder envpBuilder;
    envpBuilder.addNew(ldLibraryPathEnv.c_str());
    envpBuilder.addNew(icuDataPathEnv.c_str());
    envpBuilder.addNew(crashFilePathEnv.c_str());
    auto envp = envpBuilder.build();
    {
        std::string executableName = executablePath + '/' + "libweexjsb64.so";
        chmod(executableName.c_str(), 0755);
        const char* argv[] = { executableName.c_str(), fdStr, traceEnable ? "1" : "0", nullptr };
        if (-1 == execve(argv[0], const_cast<char* const*>(&argv[0]), const_cast<char* const*>(envp.get()))) {
            LOGE("execve failed: %s", strerror(errno));
        }
    }
    {
        std::string executableName = executablePath + '/' + "libweexjsb.so";
        chmod(executableName.c_str(), 0755);
#if PRINT_LOG_CACHEFILE
        mcfile << "jsengine WeexJSConnection::doExec start execve so name:" << executableName << std::endl;
 #endif
        const char* argv[] = { executableName.c_str(), fdStr, traceEnable ? "1" : "0", nullptr };
        if (-1 == execve(argv[0], const_cast<char* const*>(&argv[0]), const_cast<char* const*>(envp.get()))) {
#if PRINT_LOG_CACHEFILE
            mcfile << "execve failed:" << strerror(errno) << std::endl;
#endif
            LOGE("execve failed: %s", strerror(errno));
        }
    }
#if PRINT_LOG_CACHEFILE
    mcfile.close();
#endif
}

static void closeAllButThis(int exceptfd)
{
    DIR* dir = opendir("/proc/self/fd");
    if (!dir) {
        return;
    }
    int dirFd = dirfd(dir);
    struct dirent* cur;
    struct timespec start;
    clock_gettime(CLOCK_MONOTONIC, &start);
    while ((cur = readdir(dir))) {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        if ((now.tv_sec - start.tv_sec) > 6) {
            break;
        }
        if (!strcmp(cur->d_name, ".")
            || !strcmp(cur->d_name, "..")) {
            continue;
        }
        errno = 0;
        unsigned long curFd = strtoul(cur->d_name, nullptr, 10);
        if (errno)
            continue;
        if (curFd <= 2)
            continue;
        if ((curFd != dirFd) && (curFd != exceptfd)) {
            close(curFd);
        }
    }
    closedir(dir);
}
