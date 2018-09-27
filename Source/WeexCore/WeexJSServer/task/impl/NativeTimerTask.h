//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_TIMERTASK_H
#define WEEXV8_TIMERTASK_H


#include <WeexCore/WeexJSServer/task/WeexTask.h>

class NativeTimerTask : public WeexTask {
public:
    explicit NativeTimerTask(const String &instanceId, uint32_t function, int taskId, bool one_shot);
    ~NativeTimerTask() override {}

    void run(WeexRuntime *runtime) override;
    std::string taskName() override { return "NativeTimerTask"; }
    uint32_t timerFunctionID() const { return timerFunction; }

private:
    uint32_t timerFunction;
    bool repeatTimer;
};


#endif //WEEXV8_TIMERTASK_H
