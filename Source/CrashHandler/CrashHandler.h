#ifndef CRASHHANDLER_H
#define CRASHHANDLER_H

#include <signal.h>
#include <string>
#include <sys/ucontext.h>

namespace crash_handler {

class CrashHandlerInfo {
public:
    CrashHandlerInfo(std::string fileName);
    ~CrashHandlerInfo();

    void parpareFds();
    bool initializeCrashHandler();
    bool handleSignal(int signum, siginfo_t* siginfo, void* ucontext);
    bool printIP(void* addr);

    struct SignalInfo {
        int signum;
        const char* signame;
    };

private:
    void printContext();
    void printMaps();
    void printRegContent(void* addr, const char* name);
    void printUnwind();
    bool ensureDirectory(const char* base);
    void printf(const char* fmt, ...);
    void saveFileContent();

    /* static members */
    static const size_t BUF_SIZE = 1024;
    static const int maxUnwindCount = 32;

    /* members */
    struct sigaction m_sigaction[16];

    // File descriptior to store crash dump message
    int m_dumpFileFd;

    // File descriptior for /prco/self/maps
    int m_mapsFileFd;
    int m_unwinded;

    std::string m_dumpFileName;
    std::string m_fileContent;
    mcontext_t m_mcontext;
};
}
#endif /* CRASHHANDLER_H */
