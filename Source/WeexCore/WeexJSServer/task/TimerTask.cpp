//
// Created by Darin on 12/06/2018.
//

#include "TimerTask.h"
#include "../utils/Utils.h"

TimerTask::TimerTask(String id, JSC::JSValue function, uint64_t millSecTimeout, bool repeat) {
    this->taskId = genTaskId();
    this->instanceID = id;
    this->function = function;
    this->timeout = millSecTimeout;
    this->when = microTime() + millSecTimeout * TIMESPCE;
    this->repeat = repeat;
}

TimerTask::TimerTask(const TimerTask *timerTask) {
    this->taskId = timerTask->taskId;
    this->instanceID = timerTask->instanceID;
    this->repeat = timerTask->repeat;
    this->timeout = timerTask->timeout;
    this->function = timerTask->function;
    this->when = microTime() + timerTask->timeout * TIMESPCE;
}

TimerTask::TimerTask(const TimerTask &timerTask) {
    this->taskId = timerTask.taskId;
    this->instanceID = timerTask.instanceID;
    this->repeat = timerTask.repeat;
    this->timeout = timerTask.timeout;
    this->function = timerTask.function;
    this->when = microTime() + timerTask.timeout * TIMESPCE;
}
