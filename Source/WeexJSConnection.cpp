#include "WeexJSConnection.h"
#include "IPCException.h"
#include "IPCLog.h"
#include "IPCSender.h"
#include "LogUtils.h"
#include "Trace.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static void doExec(int fd, bool traceEnable);
static void closeAllButThis(int fd);

struct WeexJSConnection::WeexJSConnectionImpl {
    std::unique_ptr<IPCSender> serverSender;
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

IPCSender* WeexJSConnection::start(IPCHandler* handler)
{
    int handles[2];
    if (-1 == socketpair(AF_UNIX, SOCK_STREAM, 0, handles)) {
        throw IPCException("failed to init socketpair: %s", strerror(errno));
    }
    std::unique_ptr<IPCSender> sender(createIPCSender(handles[0], handler, true));
    m_impl->serverSender = std::move(sender);
    pid_t child = fork();
    if (child == -1) {
        int myerrno = errno;
        close(handles[1]);
        throw IPCException("failed to fork: %s", strerror(myerrno));
    } else if (child == 0) {
        // the child
        closeAllButThis(handles[1]);
        // implements close all but handles[1]
        // do exec
        doExec(handles[1], base::debug::TraceEvent::isEnable());
        LOGE("exec Failed completely.");
        // failed to exec
        _exit(1);
    } else {
        close(handles[1]);
        m_impl->child = child;
    }
    return m_impl->serverSender.get();
}

void WeexJSConnection::end()
{
    m_impl->serverSender.reset();
    if (m_impl->child) {
        int wstatus;
        pid_t child;
        while (true) {
            child = waitpid(m_impl->child, &wstatus, 0);
            if (child != -1)
                break;
            if (errno != EINTR)
                break;
        }
    }
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
            executablePath = executablePath.substr(0, executablePath.rfind('/'));
        }
        if (!executablePath.empty()
            && !icuDataPath.empty()) {
            break;
        }
    }
    fclose(f);
    return;
}

void doExec(int fd, bool traceEnable)
{
    std::string executablePath;
    std::string icuDataPath;
    findPath(executablePath, icuDataPath);
    if (executablePath.empty()) {
        LOGE("executablePath is empty");
        return;
    }
    if (icuDataPath.empty()) {
        LOGE("icuDataPath is empty");
        return;
    }
    std::string ld_library_path("LD_LIBRARY_PATH=");
    std::string icu_data_path("ICU_DATA_PATH=");
    ld_library_path.append(executablePath);
    icu_data_path.append(icuDataPath);
    char fdStr[16];
    snprintf(fdStr, 16, "%d", fd);
    const char* envp[] = { ld_library_path.c_str(), icu_data_path.c_str(), nullptr };
    {
        std::string executableName = executablePath + '/' + "libweexjsserverstub64.so";
        const char* argv[] = { executableName.c_str(), fdStr, traceEnable ? "1" : "0", nullptr };
        if (-1 == execve(argv[0], const_cast<char* const*>(&argv[0]), const_cast<char* const*>(envp))) {
            LOGE("execve failed: %s", strerror(errno));
        }
    }
    {
        std::string executableName = executablePath + '/' + "libweexjsserverstub.so";
        const char* argv[] = { executableName.c_str(), fdStr, traceEnable ? "1" : "0", nullptr };
        if (-1 == execve(argv[0], const_cast<char* const*>(&argv[0]), const_cast<char* const*>(envp))) {
            LOGE("execve failed: %s", strerror(errno));
        }
    }
}

static void closeAllButThis(int exceptfd)
{
    DIR* dir = opendir("/proc/self/fd");
    if (!dir) {
        return;
    }
    int dirFd = dirfd(dir);
    struct dirent* cur;
    while ((cur = readdir(dir))) {
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
