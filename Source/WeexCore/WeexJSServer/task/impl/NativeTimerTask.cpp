//
// Created by Darin on 20/07/2018.
//

#include "NativeTimerTask.h"

NativeTimerTask::NativeTimerTask(const String &instanceId, JSC::JSValue function) : WeexTask(instanceId) {
    this->timerFunction = function;
}

void NativeTimerTask::run(WeexRuntime *runtime) {
    runtime->exeTimerFunction(instanceId, timerFunction);
}
