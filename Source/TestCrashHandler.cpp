#include "CrashHandler.h"
#include <stdlib.h>
#include <string.h>
#if defined(__clang__)
#pragma clang optimize off
#endif

static void doCrash(int c);

static void doCrash2(int c)
{
    if (!c) {
        __builtin_trap();
    }
    doCrash(c - 1);
}

static void doCrash(int c)
{
    if (!c) {
        __builtin_trap();
    }
    if (c & 1) {
        doCrash2(c - 1);
        return;
    }
    doCrash(c - 1);
}

int main()
{
    crash_handler::initializeCrashHandler(".");
    doCrash(10);
    return 0;
}
