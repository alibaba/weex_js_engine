//
// Created by Darin on 20/07/2018.
//

#include "TimerTask.h"

TimerTask::TimerTask(const String &instanceId, JSC::JSValue function) : WeexTask(instanceId) {
    this->timerFunction = function;
}

void TimerTask::run(WeexRuntime *runtime) {
    runtime->exeTimerFunction(instanceId, timerFunction);
}
