//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_WEEXTASK_H
#define WEEXV8_WEEXTASK_H


#include <WeexCore/WeexJSServer/utils/WeexRuntime.h>
#include <WeexCore/WeexJSServer/utils/ThreadLocker.h>

class WeexTask {

public:
    String instanceId;

    WeexJSResult *result = nullptr;
public:

    explicit WeexTask(const String &instanceId) { this->instanceId = instanceId; };

    virtual ~WeexTask() = default;

    virtual void run(WeexRuntime *runtime) = 0;

    void setResult(WeexJSResult *result);

    WeexJSResult *waitResult();

private:
    ThreadLocker threadLocker;

};


#endif //WEEXV8_WEEXTASK_H
