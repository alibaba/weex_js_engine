//
// Created by yxp on 2018/6/15.
//

#include "script_side_in_simple.h"
#include "WeexCore/WeexJSServer/utils/WeexRuntime.h"

namespace weex {
namespace bridge {
namespace js {
int ScriptSideInSimple::InitFramework(
    const char *script, std::vector<INIT_FRAMEWORK_PARAMS *> &params) {
  LOGD("ScriptSideInSimple::InitFramework");

  return runtime_->initFramework(String::fromUTF8(script), params);
}

int ScriptSideInSimple::InitAppFramework(
    const char *instanceId, const char *appFramework,
    std::vector<INIT_FRAMEWORK_PARAMS *> &params) {
  LOGD("ScriptSideInSimple::InitAppFramework");
  return runtime_->initAppFramework(String::fromUTF8(instanceId),
                                    String::fromUTF8(appFramework), params);
}

int ScriptSideInSimple::CreateAppContext(const char *instanceId,
                                         const char *jsBundle) {
  LOGD("ScriptSideInSimple::CreateAppContext");
  return runtime_->createAppContext(String::fromUTF8(instanceId),
                                    String::fromUTF8(jsBundle));
}

char *ScriptSideInSimple::ExecJSOnAppWithResult(const char *instanceId,
                                                const char *jsBundle) {
  LOGD("ScriptSideInSimple::ExecJSOnAppWithResult");
  return runtime_->exeJSOnAppWithResult(String::fromUTF8(instanceId),
                                        String::fromUTF8(jsBundle));
}

int ScriptSideInSimple::CallJSOnAppContext(
    const char *instanceId, const char *func,
    std::vector<VALUE_WITH_TYPE *> &params) {
  LOGD("ScriptSideInSimple::CallJSOnAppContext");
  return runtime_->callJSOnAppContext(String::fromUTF8(instanceId),
                                      String::fromUTF8(func), params);
}

int ScriptSideInSimple::DestroyAppContext(const char *instanceId) {
  LOGD("ScriptSideInSimple::DestroyAppContext");
  return runtime_->destroyAppContext(String::fromUTF8(instanceId));
}

int ScriptSideInSimple::ExecJsService(const char *source) {
  LOGD("ScriptSideInSimple::ExecJsService");
  return runtime_->exeJsService(String::fromUTF8(source));
}

int ScriptSideInSimple::ExecTimeCallback(const char *source) {
  LOGD("ScriptSideInSimple::ExecTimeCallback");
  return runtime_->exeCTimeCallback(String::fromUTF8(source));
}

int ScriptSideInSimple::ExecJS(const char *instanceId, const char *nameSpace,
                               const char *func,
                               std::vector<VALUE_WITH_TYPE *> &params) {
  LOGD("ScriptSideInSimple::ExecJS");
  return runtime_->exeJS(String::fromUTF8(instanceId),
                         String::fromUTF8(nameSpace), String::fromUTF8(func),
                         params);
}

WeexJSResult ScriptSideInSimple::ExecJSWithResult(
    const char *instanceId, const char *nameSpace, const char *func,
    std::vector<VALUE_WITH_TYPE *> &params) {
  LOGD("ScriptSideInSimple::ExecJSWithResult");
  return runtime_->exeJSWithResult(String::fromUTF8(instanceId),
                                   String::fromUTF8(nameSpace),
                                   String::fromUTF8(func), params);
}

int ScriptSideInSimple::CreateInstance(const char *instanceId, const char *func,
                                       const char *script, const char *opts,
                                       const char *initData,
                                       const char *extendsApi) {
  LOGD(
      "CreateInstance id = %s, func = %s, script = %s, opts = %s, initData = "
      "%s, extendsApi = %s",
      instanceId, func, script, opts, initData, extendsApi);

  return runtime_->createInstance(
      String::fromUTF8(instanceId), String::fromUTF8(func),
      String::fromUTF8(script), String::fromUTF8(opts),
      String::fromUTF8(initData), String::fromUTF8(extendsApi));
}

char *ScriptSideInSimple::ExecJSOnInstance(const char *instanceId,
                                           const char *script) {
  LOGD("ScriptSideInSimple::ExecJSOnInstance");
  return runtime_->exeJSOnInstance(String::fromUTF8(instanceId),
                                   String::fromUTF8(script));
}

int ScriptSideInSimple::DestroyInstance(const char *instanceId) {
  LOGD("ScriptSideInSimple::DestroyInstance");
  return runtime_->destroyInstance(String::fromUTF8(instanceId));
}

int ScriptSideInSimple::UpdateGlobalConfig(const char *config) {
  LOGD("ScriptSideInSimple::UpdateGlobalConfig");
  return runtime_->updateGlobalConfig(String::fromUTF8(config));
}
}  // namespace js
}  // namespace bridge
}  // namespace weex
