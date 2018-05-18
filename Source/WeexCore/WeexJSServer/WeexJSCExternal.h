#ifndef _WEEX_JSC_EXTERNAL_H_
#define _WEEX_JSC_EXTERNAL_H_

#include <jni.h>
#include <sys/types.h>
#include <WeexApiHeader.h>

namespace WEEXCORE {

extern "C"  WEEX_CORE_JS_SERVER_API_FUNCTIONS* exchangeMethod(WEEX_CORE_JS_API_FUNCTIONS *functions);

}
#endif //_WEEX_JSC_EXTERNAL_H_