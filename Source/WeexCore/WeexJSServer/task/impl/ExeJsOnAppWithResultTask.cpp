//
// Created by Darin on 20/07/2018.
//

#include "ExeJsOnAppWithResultTask.h"

ExeJsOnAppWithResultTask::ExeJsOnAppWithResultTask(const String &instanceId, const String &script) : WeexTask(
        instanceId) {
    this->script = script;
}

void ExeJsOnAppWithResultTask::run(WeexRuntime *runtime) {
    auto ptr = runtime->exeJSOnAppWithResult(instanceId, script);
    if (future() != nullptr) {
        future()->setResult(ptr);
    }
}
