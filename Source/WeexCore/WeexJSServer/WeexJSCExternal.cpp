#include "WeexJSCExternal.h"
#include "WeexCore/WeexJSServer/utils/LogUtils.h"
#include "WeexJSServerApi.h"


WEEX_CORE_JS_API_FUNCTIONS *weex_core_js_api_functions = nullptr;

namespace WEEXCORE {

    extern "C" WEEX_CORE_JS_SERVER_API_FUNCTIONS *exchangeMethod(WEEX_CORE_JS_API_FUNCTIONS *functions) {
        weex_core_js_api_functions = functions;

        WEEX_CORE_JS_SERVER_API_FUNCTIONS tempApis = {
                initFramework,
                initAppFramework,
                createAppContext,
                exeJSOnAppWithResult,
                callJSOnAppContext,
                destroyAppContext,
                exeJsService,
                exeCTimeCallback,
                exeJS,
                exeJSWithResult,
                createInstance,
                exeJSOnInstance,
                destroyInstance,
                updateGlobalConfig
        };


        size_t count = sizeof(WEEX_CORE_JS_SERVER_API_FUNCTIONS);
        auto *server_api_functions = (WEEX_CORE_JS_SERVER_API_FUNCTIONS *) malloc(count);

        if (!server_api_functions)
            return nullptr;

        memset(server_api_functions, 0, count);
        memcpy(server_api_functions, &tempApis, count);

        LOGE("exchangeMethod");
        return server_api_functions;
    }
}