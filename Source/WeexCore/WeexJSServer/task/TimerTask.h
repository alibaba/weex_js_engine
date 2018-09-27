//
// Created by Darin on 12/06/2018.
//

#ifndef WEEXV8_TIMERTASK_H
#define WEEXV8_TIMERTASK_H


#include <cstdint>
#include "WTFString.h"
#include "JSCJSValue.h"
#include "WeexCore/WeexJSServer/object/WeexGlobalObject.h"


class TimerTask {

public:
    int taskId;

    String instanceID;
    uint32_t m_function;

    uint64_t timeout;
    uint64_t when;

    bool repeat = false;

    WeexGlobalObject* global_object_;

    explicit TimerTask(String id, uint32_t function, uint64_t millSecTimeout, WeexGlobalObject* global_object, bool repeat = false);

    explicit TimerTask(TimerTask *timerTask);

    TimerTask(TimerTask &timerTask);

private:


};


#endif //WEEXV8_TIMERTASK_H
