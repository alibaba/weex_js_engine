//
// Created by Darin on 20/07/2018.
//

#include "UpdateGlobalConfigTask.h"

UpdateGlobalConfigTask::UpdateGlobalConfigTask(const String &script) : WeexTask("") {
    this->script = script;
}

void UpdateGlobalConfigTask::run(WeexRuntime *runtime) {
    runtime->updateGlobalConfig(script);
}
