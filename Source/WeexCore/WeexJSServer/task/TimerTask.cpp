//
// Created by Darin on 12/06/2018.
//

#include "TimerTask.h"
#include "../utils/Utils.h"

TimerTask::TimerTask(String id, uint32_t function, uint64_t millSecTimeout, WeexGlobalObject* global_object, bool repeat) {
    this->taskId = genTaskId();
    this->instanceID = id;
    this->m_function = function;
    this->timeout = millSecTimeout;
    this->when = microTime() + millSecTimeout * TIMESPCE;
    this->repeat = repeat;
    this->global_object_ = global_object;
}

TimerTask::TimerTask(TimerTask *timerTask) {
    this->taskId = timerTask->taskId;
    this->instanceID = timerTask->instanceID;
    this->repeat = timerTask->repeat;
    this->timeout = timerTask->timeout;
    this->m_function = timerTask->m_function;
    this->when = microTime() + timerTask->timeout * TIMESPCE;
    this->global_object_ = timerTask->global_object_;
}

TimerTask::TimerTask(TimerTask &timerTask) {
    this->taskId = timerTask.taskId;
    this->instanceID = timerTask.instanceID;
    this->repeat = timerTask.repeat;
    this->timeout = timerTask.timeout;
    this->m_function = timerTask.m_function;
    this->when = microTime() + timerTask.timeout * TIMESPCE;
    this->global_object_ = timerTask.global_object_;
}
