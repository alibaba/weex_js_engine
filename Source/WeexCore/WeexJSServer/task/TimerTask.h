//
// Created by Darin on 12/06/2018.
//

#ifndef WEEXV8_TIMERTASK_H
#define WEEXV8_TIMERTASK_H


#include <cstdint>
#include "WTFString.h"
#include "JSCJSValue.h"



class TimerTask {

public:
    int taskId;

    String instanceID;
    JSC::JSValue function;

    uint64_t timeout;
    uint64_t when;

    bool repeat = false;

    explicit TimerTask(String id, JSC::JSValue function, uint64_t millSecTimeout, bool repeat = false);

    explicit TimerTask(const TimerTask *timerTask);

    TimerTask(const TimerTask &timerTask);

private:


};


#endif //WEEXV8_TIMERTASK_H
