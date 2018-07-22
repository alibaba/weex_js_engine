//
// Created by Darin on 20/07/2018.
//

#include "ExeJsOnInstanceTask.h"

ExeJsOnInstanceTask::ExeJsOnInstanceTask(const String &instanceId, const String &script) : WeexTask(
        instanceId) {
    this->script = script;
}

void ExeJsOnInstanceTask::run(WeexRuntime *runtime) {
    runtime->exeJSOnInstance(instanceId, script);
}
