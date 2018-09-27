//
// Created by Darin on 20/07/2018.
//

#include "NativeTimerTask.h"

NativeTimerTask::NativeTimerTask(const String &instanceId, uint32_t function, int taskId, bool one_shot)
        : WeexTask(instanceId, taskId) {
    this->timerFunction = function;
    repeatTimer = !one_shot;
}

void NativeTimerTask::run(WeexRuntime *runtime) {
//    LOGE("NativeTimerTask is running");
    runtime->exeTimerFunction(instanceId, timerFunction, global_object());
    // if not repeatTimer, delete the js timer function now
    if (!repeatTimer)
        runtime->removeTimerFunction(timerFunction, global_object());
}
