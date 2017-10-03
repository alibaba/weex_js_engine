#include <android/log.h>

extern "C" {
int serverMain(int argc, char** argv);
}

int main(int argc, char** argv)
{
    __android_log_print(ANDROID_LOG_INFO, "jsengine", "start stub");
    return serverMain(argc, argv);
}

