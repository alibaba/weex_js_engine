#ifndef IPCLOG_H
#define IPCLOG_H
#include <android/log.h>
#define TAG "linzj_IPC"
#define IPC_LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#endif /* IPCLOG_H */
