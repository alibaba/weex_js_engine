//
// Created by yxp on 2018/6/15.
//

#include "core_side_in_multi_so.h"
#include "WeexCore/WeexJSServer/object/Args.h"
#include "include/WeexApiHeader.h"

namespace weex {
namespace bridge {
namespace js {

void CoreSideInMultiSo::CallNative(const char *page_id, const char *task,
                                   const char *callback) {
  functions_exposed_by_core_->funcCallNative(page_id, task, callback);
}

std::unique_ptr<IPCResult> CoreSideInMultiSo::CallNativeModule(
    const char *page_id, const char *module, const char *method,
    const char *arguments, int arguments_length, const char *options,
    int options_length) {
  return functions_exposed_by_core_->funcCallNativeModule(
      page_id, module, method, arguments, arguments_length, options,
      options_length);
}

void CoreSideInMultiSo::CallNativeComponent(const char *page_id,
                                            const char *ref, const char *method,
                                            const char *arguments,
                                            int arguments_length,
                                            const char *options,
                                            int options_length) {
  functions_exposed_by_core_->funcCallNativeComponent(
      page_id, ref, method, arguments, arguments_length, options,
      options_length);
}

void CoreSideInMultiSo::AddElement(const char *page_id, const char *parent_ref,
                                   const char *dom_str, int dom_str_length,
                                   const char *index_str) {
  functions_exposed_by_core_->funcCallAddElement(page_id, parent_ref, dom_str, dom_str_length,
                                                 index_str);
}

void CoreSideInMultiSo::SetTimeout(const char *callback_id, const char *time) {
  functions_exposed_by_core_->funcSetTimeout(callback_id, time);
}

void CoreSideInMultiSo::NativeLog(const char *str_array) {
  functions_exposed_by_core_->funcCallNativeLog(str_array);
}

void CoreSideInMultiSo::CreateBody(const char *page_id, const char *dom_str,
                                   int dom_str_length) {
  functions_exposed_by_core_->funcCallCreateBody(page_id, dom_str, dom_str_length);
}

int CoreSideInMultiSo::UpdateFinish(const char *page_id, const char *task,
                                    int task_length, const char *callback,
                                    int callback_length) {
  return functions_exposed_by_core_->funcCallUpdateFinish(page_id, task, task_length,
                                                          callback, callback_length);
}

void CoreSideInMultiSo::CreateFinish(const char *page_id) {
  functions_exposed_by_core_->funcCallCreateFinish(page_id);
}

int CoreSideInMultiSo::RefreshFinish(const char *page_id, const char *task,
                                     const char *callback) {
  return functions_exposed_by_core_->funcCallRefreshFinish(page_id, task,
                                                           callback);
}

void CoreSideInMultiSo::UpdateAttrs(const char *page_id, const char *ref,
                                    const char *data, int data_length) {
  functions_exposed_by_core_->funcCallUpdateAttrs(page_id, ref, data, data_length);
}

void CoreSideInMultiSo::UpdateStyle(const char *page_id, const char *ref,
                                    const char *data, int data_length) {
  functions_exposed_by_core_->funcCallUpdateStyle(page_id, ref, data, data_length);
}

void CoreSideInMultiSo::RemoveElement(const char *page_id, const char *ref) {
  functions_exposed_by_core_->funcCallRemoveElement(page_id, ref);
}

void CoreSideInMultiSo::MoveElement(const char *page_id, const char *ref,
                                    const char *parent_ref, int index) {
  functions_exposed_by_core_->funcCallMoveElement(page_id, ref, parent_ref,
                                                  index);
}

void CoreSideInMultiSo::AddEvent(const char *page_id, const char *ref,
                                 const char *event) {
  functions_exposed_by_core_->funcCallAddEvent(page_id, ref, event);
}

void CoreSideInMultiSo::RemoveEvent(const char *page_id, const char *ref,
                                    const char *event) {
  functions_exposed_by_core_->funcCallRemoveEvent(page_id, ref, event);
}

const char *CoreSideInMultiSo::CallGCanvasLinkNative(const char *context_id,
                                                     int type,
                                                     const char *arg) {
  return functions_exposed_by_core_->funcCallGCanvasLinkNative(context_id, type,
                                                               arg);
}

int CoreSideInMultiSo::SetInterval(const char *page_id, const char *callback_id,
                                   const char *time) {
  return functions_exposed_by_core_->funcSetInterval(page_id, callback_id,
                                                     time);
}

void CoreSideInMultiSo::ClearInterval(const char *page_id,
                                      const char *callback_id) {
  functions_exposed_by_core_->funcClearInterval(page_id, callback_id);
}

const char *CoreSideInMultiSo::CallT3DLinkNative(int type, const char *arg) {
  return functions_exposed_by_core_->funcT3dLinkNative(type, arg);
}

void CoreSideInMultiSo::PostMessage(const char *vim_id, const char *data) {
  functions_exposed_by_core_->funcCallHandlePostMessage(vim_id, data);
}

void CoreSideInMultiSo::DispatchMessage(const char *client_id, const char *data,
                                        const char *callback,
                                        const char *vm_id) {
  functions_exposed_by_core_->funcCallDIspatchMessage(client_id, data, callback,
                                                      vm_id);
}

void CoreSideInMultiSo::ReportException(const char *page_id, const char *func,
                                        const char *exception_string) {
  functions_exposed_by_core_->funcReportException(page_id, func,
                                                  exception_string);
}

void CoreSideInMultiSo::SetJSVersion(const char *js_version) {
  functions_exposed_by_core_->funcSetJSVersion(js_version);
}
}  // namespace js
}  // namespace bridge
}  // namespace weex