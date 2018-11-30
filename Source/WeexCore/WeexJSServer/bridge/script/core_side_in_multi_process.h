//
// Created by yxp on 2018/6/15.
//

#ifndef WEEXV8_MULTI_PROCESS_CORE_SIDE_H
#define WEEXV8_MULTI_PROCESS_CORE_SIDE_H

#include <WeexCore/WeexJSServer/object/WeexIPCClient.h>
#include "core/bridge/script_bridge.h"
#include <memory>

class WeexJSServer;
namespace weex {
    namespace bridge {
        namespace js {
            class CoreSideInMultiProcess : public WeexCore::ScriptBridge::CoreSide {
            public:
                explicit CoreSideInMultiProcess(WeexIPCClient *client);
                explicit CoreSideInMultiProcess();

                ~CoreSideInMultiProcess() override;

                inline void set_ipc_client(WeexIPCClient *client) { client_ = client; };

                void CallNative(const char *page_id, const char *task,
                                const char *callback) override;

                std::unique_ptr<ValueWithType> CallNativeModule(
                        const char *page_id, const char *module, const char *method,
                        const char *arguments, int arguments_length, const char *options,
                        int options_length) override;

                void CallNativeComponent(const char *page_id, const char *ref,
                                         const char *method, const char *arguments,
                                         int arguments_length, const char *options,
                                         int options_length) override;

                void AddElement(const char *page_id, const char *parent_ref,
                                const char *dom_str, int dom_str_length,
                                const char *index_str) override;

                void SetTimeout(const char *callback_id, const char *time) override;

                void NativeLog(const char *str_array) override;

                void CreateBody(const char *page_id, const char *dom_str,
                                int dom_str_length) override;

                int UpdateFinish(const char *page_id, const char *task, int task_length,
                                 const char *callback, int callback_length) override;

                void CreateFinish(const char *page_id) override;

                int RefreshFinish(const char *page_id, const char *task,
                                  const char *callback) override;

                void UpdateAttrs(const char *page_id, const char *ref, const char *data,
                                 int data_length) override;

                void UpdateStyle(const char *page_id, const char *ref, const char *data,
                                 int data_length) override;

                void RemoveElement(const char *page_id, const char *ref) override;

                void MoveElement(const char *page_id, const char *ref, const char *parent_ref,
                                 int index) override;

                void AddEvent(const char *page_id, const char *ref,
                              const char *event) override;

                void RemoveEvent(const char *page_id, const char *ref,
                                 const char *event) override;

                const char *CallGCanvasLinkNative(const char *context_id, int type,
                                                  const char *arg) override;

                int SetInterval(const char *page_id, const char *callback_id,
                                const char *time) override;

                void ClearInterval(const char *page_id, const char *callback_id) override;

                const char *CallT3DLinkNative(int type, const char *arg) override;

                void PostMessage(const char *vim_id, const char *data, int dataLength) override;

                void DispatchMessage(const char *client_id, const char *data, int dataLength,
                                     const char *callback, const char *vm_id) override;

                std::unique_ptr<WeexJSResult> DispatchMessageSync(
                    const char *client_id, const char *data, int dataLength,
                    const char *vm_id) override;

                void ReportException(const char *page_id, const char *func,
                                     const char *exception_string) override;

                void SetJSVersion(const char *js_version) override;

                void OnReceivedResult(long callback_id, std::unique_ptr<WeexJSResult>& result);
                void UpdateComponentData(const char* page_id, const char* cid, const char* json_data);

            private:
                WeexIPCClient *client_;
                DISALLOW_COPY_AND_ASSIGN(CoreSideInMultiProcess);
            };
        }  // namespace js
    }  // namespace bridge
}  // namespace weex

#endif  // WEEXV8_MULTI_PROCESS_CORE_SIDE_H
