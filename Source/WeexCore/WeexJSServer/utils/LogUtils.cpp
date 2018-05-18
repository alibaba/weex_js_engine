
#include "LogUtils.h"

namespace Weex{
  bool LogUtil::mDebugMode = false;
  void LogUtil::setDebugMode(bool debug){
    mDebugMode = debug;
  }
  void LogUtil::ConsoleLogPrint(int level, const char* tag, const char* log) {
        // Log = 1, 
        // Warning = 2,
        // Error = 3,
        // Debug = 4,
        // Info = 5,
        
        // if (!mDebugMode) {
        //   LOGE("LogUtil mDebugMode is false");
        //   return;
        // }
        switch(level) {
          case 1:
            if (mDebugMode) {
              __android_log_print(ANDROID_LOG_VERBOSE, tag,"%s", log);
            }
            break;
          case 2:
            __android_log_print(ANDROID_LOG_WARN, tag,"%s", log);
            break;
          case 3:
            __android_log_print(ANDROID_LOG_ERROR, tag,"%s", log);
            break;
          case 4:
            if (mDebugMode) {
              __android_log_print(ANDROID_LOG_DEBUG, tag,"%s", log);
            }
            break;
          case 5:
            if (mDebugMode) {
              __android_log_print(ANDROID_LOG_INFO, tag,"%s", log);
            }
            break;
          default:
            if (mDebugMode) {
              __android_log_print(ANDROID_LOG_VERBOSE, tag,"%s", log);
            }
        }
      }

}