//
// Created by Darin on 2018/7/22.
//

#include <WeexCore/WeexJSServer/task/impl/InitFrameworkTask.h>
#include <WeexCore/WeexJSServer/task/impl/CreateAppContextTask.h>
#include <WeexCore/WeexJSServer/task/impl/CreateInstanceTask.h>
#include <WeexCore/WeexJSServer/task/impl/CallJsOnAppContextTask.h>
#include <WeexCore/WeexJSServer/task/impl/DestoryAppContextTask.h>
#include <WeexCore/WeexJSServer/task/impl/DestoryInstanceTask.h>
#include <WeexCore/WeexJSServer/task/impl/ExeJsOnAppWithResultTask.h>
#include <WeexCore/WeexJSServer/task/impl/UpdateGlobalConfigTask.h>
#include <WeexCore/WeexJSServer/task/impl/CTimeCallBackask.h>
#include <WeexCore/WeexJSServer/task/impl/ExeJsServicesTask.h>
#include <WeexCore/WeexJSServer/task/impl/ExeJsOnInstanceTask.h>
#include <WeexCore/WeexJSServer/task/impl/ExeJsTask.h>
#include <WeexCore/WeexJSServer/task/impl/TakeHeapSnapShot.h>
#include <WeexCore/WeexJSServer/task/impl/NativeTimerTask.h>
#include "script_side_in_queue.h"


namespace weex {
    namespace bridge {
        namespace js {
            int ScriptSideInQueue::InitFramework(
                    const char *script, std::vector<INIT_FRAMEWORK_PARAMS *> &params) {
                LOGD("ScriptSideInQueue::InitFramework");
//                return runtime_->initFramework(String::fromUTF8(script), params);
                weexTaskQueue_->addTask(new InitFrameworkTask(String::fromUTF8(script), params));
                weexTaskQueue_->init();
                return 1;
            }

            int ScriptSideInQueue::InitAppFramework(
                    const char *instanceId, const char *appFramework,
                    std::vector<INIT_FRAMEWORK_PARAMS *> &params) {
                LOGD("ScriptSideInQueue::InitAppFramework");
                weexTaskQueue_->addTask(new InitFrameworkTask(String::fromUTF8(instanceId),
                                                              String::fromUTF8(appFramework), params));
                return 1;

//                return runtime_->initAppFramework(String::fromUTF8(instanceId),
//                                                  String::fromUTF8(appFramework), params);
            }

            int ScriptSideInQueue::CreateAppContext(const char *instanceId,
                                                    const char *jsBundle) {
                LOGD("ScriptSideInQueue::CreateAppContext");
                auto script = String::fromUTF8(jsBundle);
                if(script.isEmpty()) {
                    return 0;
                }
                weexTaskQueue_->addTask(new CreateAppContextTask(String::fromUTF8(instanceId),
                                                                 script));
                return 1;
            }

            std::unique_ptr<WeexJSResult> ScriptSideInQueue::ExecJSOnAppWithResult(const char *instanceId,
                                                           const char *jsBundle) {
                LOGD("ScriptSideInQueue::ExecJSOnAppWithResult");

                WeexTask *task = new ExeJsOnAppWithResultTask(String::fromUTF8(instanceId),
                                                              String::fromUTF8(jsBundle));

                auto future = std::unique_ptr<WeexTask::Future>(new WeexTask::Future());
                task->set_future(future.get());

                weexTaskQueue_->addTask(task);

                return std::move(future->waitResult());
            }

            int ScriptSideInQueue::CallJSOnAppContext(
                    const char *instanceId, const char *func,
                    std::vector<VALUE_WITH_TYPE *> &params) {
                LOGD("ScriptSideInQueue::CallJSOnAppContext");

                weexTaskQueue_->addTask(new CallJsOnAppContextTask(String::fromUTF8(instanceId),
                                                                   String::fromUTF8(func), params));

                return 1;
            }

            int ScriptSideInQueue::DestroyAppContext(const char *instanceId) {
                LOGD("ScriptSideInQueue::DestroyAppContext");

                weexTaskQueue_->addTask(new DestoryAppContextTask(String::fromUTF8(instanceId)));

                return 1;
            }

            int ScriptSideInQueue::ExecJsService(const char *source) {
                LOGD("ScriptSideInQueue::ExecJsService");

                weexTaskQueue_->addTask(new ExeJsServicesTask(String::fromUTF8(source)));

                return 1;
            }

            int ScriptSideInQueue::ExecTimeCallback(const char *source) {
                LOGD("ScriptSideInQueue::ExecTimeCallback");

                weexTaskQueue_->addTask(new CTimeCallBackask(String::fromUTF8(source)));

                return 1;
            }

            int ScriptSideInQueue::ExecJS(const char *instanceId, const char *nameSpace,
                                          const char *func,
                                          std::vector<VALUE_WITH_TYPE *> &params) {
                LOGD("ScriptSideInQueue::ExecJS");

                ExeJsTask *task = new ExeJsTask(instanceId, params);

                task->addExtraArg(String::fromUTF8(nameSpace));
                task->addExtraArg(String::fromUTF8(func));

                weexTaskQueue_->addTask(task);

                return 1;
            }

            std::unique_ptr<WeexJSResult>  ScriptSideInQueue::ExecJSWithResult(
                    const char *instanceId, const char *nameSpace, const char *func,
                    std::vector<VALUE_WITH_TYPE *> &params) {
                LOGD("ScriptSideInQueue::ExecJSWithResult");

                ExeJsTask *task = new ExeJsTask(instanceId, params, true);

                auto future = std::unique_ptr<WeexTask::Future>(new WeexTask::Future());
                task->set_future(future.get());

                task->addExtraArg(String::fromUTF8(nameSpace));
                task->addExtraArg(String::fromUTF8(func));
                weexTaskQueue_->addTask(task);
                return std::move(future->waitResult());
            }

            void  ScriptSideInQueue::ExecJSWithCallback(
                    const char *instanceId, const char *nameSpace, const char *func,
                    std::vector<VALUE_WITH_TYPE *> &params, long callback_id) {
                LOGD("ScriptSideInQueue::ExecJSWithCallback");

                ExeJsTask *task = new ExeJsTask(instanceId, params, callback_id);

                task->addExtraArg(String::fromUTF8(nameSpace));
                task->addExtraArg(String::fromUTF8(func));
                weexTaskQueue_->addTask(task);
            }

            int ScriptSideInQueue::CreateInstance(const char *instanceId, const char *func,
                                                  const char *script, const char *opts,
                                                  const char *initData,
                                                  const char *extendsApi,
                                                  std::vector<INIT_FRAMEWORK_PARAMS*>& params) {
                LOGD(
                        "CreateInstance id = %s, func = %s, script = %s, opts = %s, initData = "
                        "%s, extendsApi = %s",
                        instanceId, func, script, opts, initData, extendsApi);

                auto string = String::fromUTF8(script);
                if(string.isEmpty()) {
                    return 0;
                }
                CreateInstanceTask *task = new CreateInstanceTask(String::fromUTF8(instanceId),
                                                                  string);

                task->addExtraArg(String::fromUTF8(func));
                task->addExtraArg(String::fromUTF8(opts));
                task->addExtraArg(String::fromUTF8(initData));
                task->addExtraArg(String::fromUTF8(extendsApi));
                task->addExtraOptionArgs(params);
                weexTaskQueue_->addTask(task);

                return 1;
            }

            std::unique_ptr<WeexJSResult> ScriptSideInQueue::ExecJSOnInstance(const char *instanceId,
                                                      const char *script) {
                LOGD("ScriptSideInQueue::ExecJSOnInstance");
                ExeJsOnInstanceTask *task = new ExeJsOnInstanceTask(String::fromUTF8(instanceId),
                                                                    String::fromUTF8(script));
                weexTaskQueue_->addTask(task);
                auto future = std::unique_ptr<WeexTask::Future>(new WeexTask::Future());
                task->set_future(future.get());
                return std::move(future->waitResult());
            }

            int ScriptSideInQueue::DestroyInstance(const char *instanceId) {
                LOGD("ScriptSideInQueue::DestroyInstance");
                weexTaskQueue_->addTask(new DestoryInstanceTask(String::fromUTF8(instanceId)));
                return 1;
            }

            int ScriptSideInQueue::UpdateGlobalConfig(const char *config) {
                LOGD("ScriptSideInQueue::UpdateGlobalConfig");
                weexTaskQueue_->addTask(new UpdateGlobalConfigTask(String::fromUTF8(config)));
                return 1;
            }
        }  // namespace js
    }  // namespace bridge

}  // namespace weex
