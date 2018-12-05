//
// Created by yxp on 2018/6/15.
//

#include <WeexCore/WeexJSServer/object/WeexEnv.h>
#include "WeexCore/WeexJSServer/bridge/script/script_bridge_in_multi_so.h"
#include "WeexCore/WeexJSServer/bridge/platform/platform_bridge_in_multi_so.h"
#include "WeexCore/WeexJSServer/bridge/script/core_side_in_multi_process.h"
#include "WeexCore/WeexJSServer/bridge/script/core_side_in_multi_so.h"
#include "WeexCore/WeexJSServer/bridge/script/core_side_in_simple.h"
#include "WeexCore/WeexJSServer/bridge/script/script_side_in_simple.h"
#include "WeexCore/WeexJSServer/utils/WeexRuntime.h"
#include "core/manager/weex_core_manager.h"
#include "WeexCore/WeexJSServer/bridge/script/script_side_in_queue.h"

static FunctionsExposedByCore *g_functions_exposed_by_core = nullptr;

extern "C" FunctionsExposedByJS *ExchangeJSBridgeFunctions(
    FunctionsExposedByCore *functions) {
  g_functions_exposed_by_core = functions;
  return weex::bridge::js::ScriptBridgeInMultiSo::GetExposedFunctions();
}

namespace weex {
namespace bridge {
namespace js {
ScriptBridgeInMultiSo *ScriptBridgeInMultiSo::g_instance = NULL;

ScriptBridgeInMultiSo::ScriptBridgeInMultiSo() {
  set_script_side(new ScriptSideInQueue());
//  set_core_side(new CoreSideInSimple());
    set_core_side(new CoreSideInMultiSo(g_functions_exposed_by_core));
  //  set_core_side(new MultiProcessCoreSide());
}

ScriptBridgeInMultiSo::~ScriptBridgeInMultiSo() {}

FunctionsExposedByJS *ScriptBridgeInMultiSo::GetExposedFunctions() {
  FunctionsExposedByJS temp = {
      InitFramework,         InitAppFramework,   CreateAppContext,
      ExecJSOnAppWithResult, CallJSOnAppContext, DestroyAppContext,
      ExecJSService,         ExecTimeCallback,   ExecJS,
      ExecJSWithResult,      ExecJSWithCallback, CreateInstance,     ExecJSOnInstance,
      DestroyInstance,       UpdateGlobalConfig};
  auto functions = (FunctionsExposedByJS *)malloc(sizeof(FunctionsExposedByJS));
  memset(functions, 0, sizeof(FunctionsExposedByJS));
  memcpy(functions, &temp, sizeof(FunctionsExposedByJS));
  return functions;
}

int ScriptBridgeInMultiSo::InitFramework(
    const char *script, std::vector<INIT_FRAMEWORK_PARAMS *> &params) {
  static_cast<ScriptSideInQueue *>(Instance()->script_side())
          ->setTaskQueue(new WeexTaskQueue(false));

  WeexEnv::getEnv()->setScriptBridge(Instance());

  return Instance()->script_side()->InitFramework(script, params);
}

int ScriptBridgeInMultiSo::InitAppFramework(
    const char *instanceId, const char *appFramework,
    std::vector<INIT_FRAMEWORK_PARAMS *> &params) {
  return Instance()->script_side()->InitAppFramework(instanceId, appFramework,
                                                 params);
}

int ScriptBridgeInMultiSo::CreateAppContext(const char *instanceId,
                                            const char *jsBundle) {
  return Instance()->script_side()->CreateAppContext(instanceId, jsBundle);
}

std::unique_ptr<WeexJSResult> ScriptBridgeInMultiSo::ExecJSOnAppWithResult(const char *instanceId,
                                                   const char *jsBundle) {
  return Instance()->script_side()->ExecJSOnAppWithResult(instanceId, jsBundle);
}

int ScriptBridgeInMultiSo::CallJSOnAppContext(
    const char *instanceId, const char *func,
    std::vector<VALUE_WITH_TYPE *> &params) {
  return Instance()->script_side()->CallJSOnAppContext(instanceId, func, params);
}

int ScriptBridgeInMultiSo::DestroyAppContext(const char *instanceId) {
  return Instance()->script_side()->DestroyAppContext(instanceId);
}

int ScriptBridgeInMultiSo::ExecJSService(const char *source) {
  return Instance()->script_side()->ExecJsService(source);
}

int ScriptBridgeInMultiSo::ExecTimeCallback(const char *source) {
  return Instance()->script_side()->ExecTimeCallback(source);
}

int ScriptBridgeInMultiSo::ExecJS(const char *instanceId, const char *nameSpace,
                                  const char *func,
                                  std::vector<VALUE_WITH_TYPE *> &params) {
  return Instance()->script_side()->ExecJS(instanceId, nameSpace, func, params);
}

std::unique_ptr<WeexJSResult>  ScriptBridgeInMultiSo::ExecJSWithResult(
    const char *instanceId, const char *nameSpace, const char *func,
    std::vector<VALUE_WITH_TYPE *> &params) {
  return Instance()->script_side()->ExecJSWithResult(instanceId, nameSpace, func,
                                                 params);
}

void ScriptBridgeInMultiSo::ExecJSWithCallback(
    const char *instanceId, const char *nameSpace, const char *func,
    std::vector<VALUE_WITH_TYPE *> &params, long callback_id) {
  Instance()->script_side()->ExecJSWithCallback(instanceId, nameSpace, func, params, callback_id);
}

int ScriptBridgeInMultiSo::CreateInstance(const char *instanceId,
                                          const char *func, const char *script,
                                          const char *opts,
                                          const char *initData,
                                          const char *extendsApi,
                                          std::vector<INIT_FRAMEWORK_PARAMS*>& params) {
  return Instance()->script_side()->CreateInstance(instanceId, func, script, opts,
                                               initData, extendsApi,params);
}

std::unique_ptr<WeexJSResult> ScriptBridgeInMultiSo::ExecJSOnInstance(const char *instanceId,
                                              const char *script) {
  return Instance()->script_side()->ExecJSOnInstance(instanceId, script);
}

int ScriptBridgeInMultiSo::DestroyInstance(const char *instanceId) {
  return Instance()->script_side()->DestroyInstance(instanceId);
}

int ScriptBridgeInMultiSo::UpdateGlobalConfig(const char *config) {
  return Instance()->script_side()->UpdateGlobalConfig(config);
}
}  // namespace js
}  // namespace bridge
}  // namespace weex