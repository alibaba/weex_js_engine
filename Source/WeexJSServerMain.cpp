#include "IPCException.h"
#include "LogUtils.h"
#include "WeexJSServer.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
int serverMain(int argc, char** argv);
}

static unsigned long parseUL(const char* s)
{
    unsigned long val;
    errno = 0;
    val = strtoul(s, nullptr, 10);
    if (errno) {
        LOGE("failed to parse ul: %s %s", s, strerror(errno));
        exit(1);
    }
    return val;
}

int serverMain(int argc, char** argv)
{
    unsigned long fd;
    unsigned long enableTrace;
    if (argc != 3) {
        LOGE("argc is not correct");
        exit(1);
    }
    fd = parseUL(argv[1]);
    enableTrace = parseUL(argv[2]);
    try {
        WeexJSServer server(static_cast<int>(fd), static_cast<bool>(enableTrace));
        server.loop();
    } catch (IPCException& e) {
        LOGE("%s", e.msg());
        exit(1);
    }
    return 0;
}
