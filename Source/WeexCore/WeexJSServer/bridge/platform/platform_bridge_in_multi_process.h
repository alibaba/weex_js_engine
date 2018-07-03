//
// Created by yxp on 2018/6/12.
//

#ifndef WEEXV8_PLATFORM_MULTI_PROCESS_BRIDGE_H
#define WEEXV8_PLATFORM_MULTI_PROCESS_BRIDGE_H

#include "core/bridge/platform_bridge.h"

class IPCHandler;
class IPCResult;
class IPCArguments;
namespace weex {
class PlatformBridgeInMultiProcess : public WeexCore::PlatformBridge {
 public:
  static PlatformBridgeInMultiProcess* Instance() {
    if (g_instance == NULL) {
      g_instance = new PlatformBridgeInMultiProcess();
    }
    return g_instance;
  }

  void RegisterIPCCallback(IPCHandler* handler);

 private:
#define IPC_METHOD(method_name) \
  static std::unique_ptr<IPCResult> method_name(IPCArguments* arguments);

  IPC_METHOD(SetDefaultHeightAndWidthIntoRootDom)
  IPC_METHOD(OnInstanceClose)
  IPC_METHOD(SetStyleWidth)
  IPC_METHOD(SetStyleHeight)
  IPC_METHOD(SetMargin)
  IPC_METHOD(SetPadding)
  IPC_METHOD(SetPosition)
  IPC_METHOD(MarkDirty)
  IPC_METHOD(SetViewPortWidth)
  IPC_METHOD(ForceLayout)
  IPC_METHOD(NotifyLayout)
  IPC_METHOD(GetFirstScreenRenderTime)
  IPC_METHOD(GetRenderFinishTime)
  IPC_METHOD(SetRenderContainerWrapContent)
  IPC_METHOD(BindMeasurementToRenderObject)
  IPC_METHOD(RegisterCoreEnv)
  IPC_METHOD(GetRenderObject)
  IPC_METHOD(UpdateRenderObjectStyle)
  IPC_METHOD(UpdateRenderObjectAttr)
  IPC_METHOD(CopyRenderObject)
  IPC_METHOD(SetMeasureFunctionAdapter)
  IPC_METHOD(SetPlatform)
  IPC_METHOD(SetDeviceWidthAndHeight)
  IPC_METHOD(AddOption)

  IPC_METHOD(InitFramework)
  IPC_METHOD(InitAppFramework)
  IPC_METHOD(CreateAppContext)
  IPC_METHOD(ExecJSOnAppWithResult)
  IPC_METHOD(CallJSOnAppContext)
  IPC_METHOD(DestroyAppContext)
  IPC_METHOD(ExecJSService)
  IPC_METHOD(ExecTimerCallback)
  IPC_METHOD(ExecJS)
  IPC_METHOD(ExecJSWithResult)
  IPC_METHOD(CreateInstance)
  IPC_METHOD(ExecJSOnInstance)
  IPC_METHOD(DestroyInstance)
  IPC_METHOD(UpdateGlobalConfig)

 private:
  PlatformBridgeInMultiProcess();
  virtual ~PlatformBridgeInMultiProcess();
  static PlatformBridgeInMultiProcess* g_instance;
  DISALLOW_COPY_AND_ASSIGN(PlatformBridgeInMultiProcess);
};
}  // namespace weex

#endif  // WEEXV8_PLATFORM_MULTI_PROCESS_BRIDGE_H
