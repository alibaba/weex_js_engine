//
// Created by yxp on 2018/6/15.
//

#ifndef WEEXV8_JS_MULTI_PROCESS_BRIDGE_H
#define WEEXV8_JS_MULTI_PROCESS_BRIDGE_H

#include "core/bridge/script_bridge.h"

class IPCHandler;
class IPCResult;
class IPCArguments;

namespace weex {
namespace bridge {
namespace js {
class ScriptBridgeInMultiProcess : public WeexCore::ScriptBridge {
 public:
  static ScriptBridgeInMultiProcess* Instance() {
    if (g_instance == NULL) {
      g_instance = new ScriptBridgeInMultiProcess();
    }
    return g_instance;
  }

  ScriptBridgeInMultiProcess();
  virtual ~ScriptBridgeInMultiProcess();
  static void RegisterIPCCallback(IPCHandler* handler);

 private:
#define IPC_METHOD(method_name) \
  static std::unique_ptr<IPCResult> method_name(IPCArguments* arguments);

  IPC_METHOD(InitFramework)
  IPC_METHOD(InitAppFramework)
  IPC_METHOD(CreateAppContext)
  IPC_METHOD(ExecJSOnAppWithResult)
  IPC_METHOD(CallJSOnAppContext)
  IPC_METHOD(DestroyAppContext)
  IPC_METHOD(ExecTimerCallback)
  IPC_METHOD(ExecJSService)
  IPC_METHOD(TakeHeapSnapshot)
  IPC_METHOD(ExecJS)
  IPC_METHOD(ExecJSWithResult)
  IPC_METHOD(CreateInstance)
  IPC_METHOD(DestroyInstance)
  IPC_METHOD(ExecJSOnInstance)
  IPC_METHOD(UpdateGlobalConfig)

 private:
  static ScriptBridgeInMultiProcess* g_instance;
  DISALLOW_COPY_AND_ASSIGN(ScriptBridgeInMultiProcess);
};
}  // namespace js
}  // namespace bridge
}  // namespace weex

#endif  // WEEXV8_JS_MULTI_PROCESS_BRIDGE_H
