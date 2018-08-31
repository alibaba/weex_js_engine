//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_WEEXTASK_H
#define WEEXV8_WEEXTASK_H


#include <WeexCore/WeexJSServer/utils/WeexRuntime.h>
#include <WeexCore/WeexJSServer/utils/ThreadLocker.h>

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
    explicit WeexTask(const String &instanceId, int taskId) : future_(nullptr) {
        this->instanceId = instanceId;
        this->taskId = taskId;
    };

    explicit WeexTask(const String &instanceId) : WeexTask(instanceId, genTaskId()) {};

    virtual ~WeexTask() = default;

    virtual void run(WeexRuntime *runtime) = 0;

    inline void set_future(Future* future) {
        future_ = future;
    }

    inline Future* future() {
        return future_;
    }

private:
    Future* future_;
};


#endif //WEEXV8_WEEXTASK_H
