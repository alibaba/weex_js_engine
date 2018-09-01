//
// Created by Darin on 12/06/2018.
//

#include "TimerTask.h"
#include "../utils/Utils.h"

TimerTask::TimerTask(String id, JSC::JSValue function, uint64_t millSecTimeout, WeexGlobalObject* global_object, bool repeat) {
    this->taskId = genTaskId();
    this->instanceID = id;
    this->function = function;
    this->timeout = millSecTimeout;
    this->when = microTime() + millSecTimeout * TIMESPCE;
    this->repeat = repeat;
    this->global_object_ = global_object;
}

TimerTask::TimerTask(const TimerTask *timerTask) {
    this->taskId = timerTask->taskId;
    this->instanceID = timerTask->instanceID;
    this->repeat = timerTask->repeat;
    this->timeout = timerTask->timeout;
    this->function = timerTask->function;
    this->when = microTime() + timerTask->timeout * TIMESPCE;
    this->global_object_ = timerTask->global_object_;
}

TimerTask::TimerTask(const TimerTask &timerTask) {
    this->taskId = timerTask.taskId;
    this->instanceID = timerTask.instanceID;
    this->repeat = timerTask.repeat;
    this->timeout = timerTask.timeout;
    this->function = timerTask.function;
    this->when = microTime() + timerTask.timeout * TIMESPCE;
    this->global_object_ = timerTask.global_object_;
}
