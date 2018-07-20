//
// Created by Darin on 20/07/2018.
//

#include "ExeJsOnInstanceTask.h"

ExeJsOnInstanceTaskWeexTask::ExeJsOnInstanceTaskWeexTask(const String &instanceId, const String &script) : WeexTask(
        instanceId) {
    this->script = script;
}

void ExeJsOnInstanceTaskWeexTask::run(WeexRuntime *runtime) {
    runtime->exeJSOnInstance(instanceId, script);
}
