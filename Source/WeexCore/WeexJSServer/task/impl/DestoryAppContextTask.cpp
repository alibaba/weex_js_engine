//
// Created by Darin on 20/07/2018.
//

#include "DestoryAppContextTask.h"

DestoryAppContextTask::DestoryAppContextTask(const String &instanceId) : WeexTask(instanceId) {}

void DestoryAppContextTask::run(WeexRuntime *runtime) {
    runtime->destroyAppContext(instanceId);
}
