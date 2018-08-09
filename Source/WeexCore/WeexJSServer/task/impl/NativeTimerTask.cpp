//
// Created by Darin on 20/07/2018.
//

#include "NativeTimerTask.h"

NativeTimerTask::NativeTimerTask(const String &instanceId, JSC::JSValue function, int taskId)
        : WeexTask(instanceId, taskId) {
    this->timerFunction = function;
}

void NativeTimerTask::run(WeexRuntime *runtime) {
    LOGE("NativeTimerTask is running");
    runtime->exeTimerFunction(instanceId, timerFunction);
}
