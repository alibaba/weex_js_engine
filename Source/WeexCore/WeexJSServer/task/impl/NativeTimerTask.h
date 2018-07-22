//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_TIMERTASK_H
#define WEEXV8_TIMERTASK_H


#include <WeexCore/WeexJSServer/task/WeexTask.h>

class TimerTask : public WeexTask {
public:
    explicit TimerTask(const String &instanceId, JSC::JSValue function);

    void run(WeexRuntime *runtime) override;

private:
    JSC::JSValue timerFunction = JSC::jsUndefined();
};


#endif //WEEXV8_TIMERTASK_H
