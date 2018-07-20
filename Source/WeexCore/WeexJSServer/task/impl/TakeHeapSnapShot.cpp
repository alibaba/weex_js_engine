//
// Created by Darin on 20/07/2018.
//

#include "TakeHeapSnapShot.h"

TakeHeapSnapShot::TakeHeapSnapShot(const String &instanceId) : WeexTask(instanceId) {}

void TakeHeapSnapShot::run(WeexRuntime *runtime) {
    WeexTask::run(runtime);
}
