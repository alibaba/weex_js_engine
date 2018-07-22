//
// Created by Darin on 2018/7/22.
//

#ifndef WEEXV8_SCRIPT_SIDE_IN_QUEUE_H
#define WEEXV8_SCRIPT_SIDE_IN_QUEUE_H

#include <core/bridge/script_bridge.h>
#include "../../task/WeexTaskQueue.h"

namespace weex {
    namespace bridge {
        namespace js {
            class ScriptSideInQueue : public WeexCore::ScriptBridge::ScriptSide {

            private:
                WeexTaskQueue *weexTaskQueue_;

            public:
                explicit ScriptSideInQueue() : weexTaskQueue_(nullptr) {};

                explicit ScriptSideInQueue(WeexTaskQueue *taskQueue) { weexTaskQueue_ = taskQueue; }

            public:

                void setTaskQueue(WeexTaskQueue *taskQueue) { weexTaskQueue_ = taskQueue; }

                int InitFramework(const char *script,
                                  std::vector<INIT_FRAMEWORK_PARAMS *> params) override;

                int InitAppFramework(const char *instanceId, const char *appFramework,
                                     std::vector<INIT_FRAMEWORK_PARAMS *> params) override;

                int CreateAppContext(const char *instanceId, const char *jsBundle) override;

                char *ExecJSOnAppWithResult(const char *instanceId,
                                            const char *jsBundle) override;

                int CallJSOnAppContext(const char *instanceId, const char *func,
                                       std::vector<VALUE_WITH_TYPE *> params) override;

                int DestroyAppContext(const char *instanceId) override;

                int ExecJsService(const char *source) override;

                int ExecTimeCallback(const char *source) override;

                int ExecJS(const char *instanceId, const char *nameSpace, const char *func,
                           std::vector<VALUE_WITH_TYPE *> params) override;

                WeexJSResult ExecJSWithResult(const char *instanceId, const char *nameSpace,
                                              const char *func,
                                              std::vector<VALUE_WITH_TYPE *> params) override;

                int CreateInstance(const char *instanceId, const char *func,
                                   const char *script, const char *opts, const char *initData,
                                   const char *extendsApi) override;

                char *ExecJSOnInstance(const char *instanceId, const char *script) override;

                int DestroyInstance(const char *instanceId) override;

                int UpdateGlobalConfig(const char *config) override;

            };
        }  // namespace js
    }  // namespace bridge

}  // namespace weex


#endif //WEEXV8_SCRIPT_SIDE_IN_QUEUE_H
