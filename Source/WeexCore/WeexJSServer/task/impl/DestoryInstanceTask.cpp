//
// Created by Darin on 20/07/2018.
//

#include "DestoryInstanceTask.h"

DestoryInstanceTask::DestoryInstanceTask(const String &instanceId) : WeexTask(instanceId) {}

void DestoryInstanceTask::run(WeexRuntime *runtime) {
    runtime->destroyInstance(instanceId);
}
