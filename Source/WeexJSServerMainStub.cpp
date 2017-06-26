#include "LogUtils.h"

extern "C" {
int serverMain(int argc, char** argv);
}

int main(int argc, char** argv)
{
    LOGE("WeexJsServerMainStub main");
    return serverMain(argc, argv);
}

