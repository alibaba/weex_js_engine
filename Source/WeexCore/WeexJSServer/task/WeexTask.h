//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_WEEXTASK_H
#define WEEXV8_WEEXTASK_H


#include <WeexCore/WeexJSServer/utils/WeexRuntime.h>
#include <WeexCore/WeexJSServer/utils/ThreadLocker.h>
#include "base/time_calculator.h"

class WeexTask {

public:

    class Future {

    public:
        Future() : has_result_(false) {}

        ~Future() {}

        void setResult(std::unique_ptr<WeexJSResult> &result);

        std::unique_ptr<WeexJSResult> waitResult();

    private:
        bool has_result_ = false;
        std::unique_ptr<WeexJSResult> result_;
        ThreadLocker thread_locker_;
    };

    String instanceId;
    int taskId;
    explicit WeexTask(const String &instanceId, int taskId) : future_(nullptr), global_object_(nullptr) {
        this->instanceId = instanceId;
        this->taskId = taskId;
        timeCalculator.reset(std::move(new weex::base::TimeCalculator(weex::base::TaskPlatform::JSS_ENGINE,"")));
    };

    explicit WeexTask(const String &instanceId) : WeexTask(instanceId, genTaskId()) {};

    virtual ~WeexTask() = default;

    virtual void run(WeexRuntime *runtime) = 0;
    virtual std::string taskName() = 0;

    inline void set_future(Future* future) {
        future_ = future;
    }

    inline Future* future() {
        return future_;
    }

    inline WeexGlobalObject* global_object() {
        return global_object_;
    }

    inline void set_global_object(WeexGlobalObject* global_object) {
        global_object_ = global_object;
    }

    std::unique_ptr<weex::base::TimeCalculator> timeCalculator;

private:
    Future* future_;
    WeexGlobalObject* global_object_;
};


#endif //WEEXV8_WEEXTASK_H
