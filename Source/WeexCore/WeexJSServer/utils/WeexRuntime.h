//
// Created by Darin on 28/04/2018.
//

#ifndef WEEXV8_JSRUNTIME_H
#define WEEXV8_JSRUNTIME_H


#include <WeexCore/WeexJSServer/object/WeexObjectHolder.h>
#include <WeexCore/WeexJSServer/object/WeexIPCClient.h>

namespace WeexCore {
    class ScriptBridge;
}
class WeexRuntime {

public:
    WeexCore::ScriptBridge* script_bridge_;
    std::unique_ptr<WeexObjectHolder> weexObjectHolder;
    std::map<std::string, WeexObjectHolder *> weexLiteAppObjectHolderMap;

    explicit WeexRuntime(bool isMultiProgress = true);

    explicit WeexRuntime(WeexCore::ScriptBridge *script_bridge, bool isMultiProgress = true);

    int initFramework(IPCArguments *arguments);
    int initFramework(const String &script, std::vector<INIT_FRAMEWORK_PARAMS *> params);

    int initAppFrameworkMultiProcess(const String &instanceId, const String &appFramework, IPCArguments *arguments);

    int
    initAppFramework(const String &instanceId, const String &appFramework, std::vector<INIT_FRAMEWORK_PARAMS *> params);

    int createAppContext(const String &instanceId, const String &jsBundle);

    char *exeJSOnAppWithResult(const String &instanceId, const String &jsBundle);

    int callJSOnAppContext(IPCArguments *arguments);

    int callJSOnAppContext(const String &instanceId, const String &func, std::vector<VALUE_WITH_TYPE *> params);

    int destroyAppContext(const String &instanceId);

    int exeJsService(const String &source);

    int exeCTimeCallback(const String &source);

    int exeJS(const String &instanceId, const String &nameSpace, const String &func, IPCArguments *arguments);

    int
    exeJS(const String &instanceId, const String &nameSpace, const String &func, std::vector<VALUE_WITH_TYPE *> params);

    WeexJSResult exeJSWithResult(const String &instanceId, const String &nameSpace, const String &func,
                          IPCArguments *arguments);

    WeexJSResult exeJSWithResult(const String &instanceId, const String &nameSpace, const String &func,
                          std::vector<VALUE_WITH_TYPE *> params);

    int createInstance(const String &instanceId, const String &func, const String &script, const String &opts,
                       const String &initData, const String &extendsApi);

    char *exeJSOnInstance(const String &instanceId, const String &script);

    int destroyInstance(const String &instanceId);

    int updateGlobalConfig(const String &config);
    int exeTimerFunction(const String &instanceId, JSC::JSValue timerFunction);
    WeexObjectHolder * getLightAppObjectHolder(const String &instanceId);

private:
    int _initFramework(const String &source);

    int _initAppFramework(const String &instanceId, const String &appFramework);

    void _getArgListFromIPCArguments(MarkedArgumentBuffer *obj, ExecState *state, IPCArguments *arguments,
                                     size_t start);

    void _getArgListFromJSParams(MarkedArgumentBuffer *obj, ExecState *state, std::vector<VALUE_WITH_TYPE *> params);

    bool is_multi_process_;
};


#endif //WEEXV8_JSRUNTIME_H
