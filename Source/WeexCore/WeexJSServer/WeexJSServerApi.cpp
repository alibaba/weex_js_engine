//
// Created by Darin on 13/02/2018.
//

#include "WeexJSServerApi.h"
#include "object/WeexObjectHolder.h"
#include "utils/WeexRuntime.h"
#include "utils/LogUtils.h"

using namespace JSC;
using namespace WTF;

static WeexRuntime *g_weexRuntime = nullptr;

extern "C" {
int initFramework(const char *script, std::vector<INIT_FRAMEWORK_PARAMS *> params) {
    g_weexRuntime = new WeexRuntime(false);

    return g_weexRuntime->initFramework(char2String(script), params);
}


int
initAppFramework(const char *instanceId, const char *appFramework, std::vector<INIT_FRAMEWORK_PARAMS *> params) {
    return g_weexRuntime->initAppFramework(char2String(instanceId), char2String(appFramework), params);
}

int createAppContext(const char *instanceId, const char *jsBundle) {
    return g_weexRuntime->createAppContext(char2String(instanceId), char2String(jsBundle));
}

char *exeJSOnAppWithResult(const char *instanceId, const char *jsBundle) {
    return g_weexRuntime->exeJSOnAppWithResult(char2String(instanceId), char2String(jsBundle));
}

int callJSOnAppContext(const char *instanceId, const char *func, std::vector<VALUE_WITH_TYPE *> params) {
    return g_weexRuntime->callJSOnAppContext(char2String(instanceId), char2String(func), params);
}

int destroyAppContext(const char *instanceId) {
    return g_weexRuntime->destroyAppContext(char2String(instanceId));
}

int exeJsService(const char *source) {
    return g_weexRuntime->exeJsService(char2String(source));
}

int exeCTimeCallback(const char *source) {
    return g_weexRuntime->exeCTimeCallback(source);
}

int exeJS(const char *instanceId, const char *nameSpace, const char *func, std::vector<VALUE_WITH_TYPE *> params) {
    return g_weexRuntime->exeJS(char2String(instanceId), char2String(nameSpace), char2String(func), params);
}

char *exeJSWithResult(const char *instanceId, const char *nameSpace, const char *func,
                      std::vector<VALUE_WITH_TYPE *> params) {
    return g_weexRuntime->exeJSWithResult(char2String(instanceId), char2String(nameSpace), char2String(func), params);
}

int createInstance(const char *instanceId, const char *func, const char *script, const char *opts,
                   const char *initData, const char *extendsApi) {
    return g_weexRuntime->createInstance(char2String(instanceId), char2String(func), char2String(script),
                                         char2String(opts), char2String(initData), char2String(extendsApi));
}

char *exeJSOnInstance(const char *instanceId, const char *script) {
    return g_weexRuntime->exeJSOnInstance(char2String(instanceId), script);
}

int destroyInstance(const char *instanceId) {
    return g_weexRuntime->destroyInstance(char2String(instanceId));
}

int updateGlobalConfig(const char *config) {
    return g_weexRuntime->updateGlobalConfig(char2String(config));
}
}