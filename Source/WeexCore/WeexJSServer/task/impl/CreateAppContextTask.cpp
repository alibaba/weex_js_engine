//
// Created by Darin on 20/07/2018.
//

#include "CreateAppContextTask.h"

CreateAppContextTask::CreateAppContextTask(const String &instanceId, const String &script) : WeexTask(instanceId) {}

void CreateAppContextTask::run(WeexRuntime *runtime) {
    runtime->createAppContext(instanceId, script);
}
