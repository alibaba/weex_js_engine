//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_INITFRAMEWORKTASK_H
#define WEEXV8_INITFRAMEWORKTASK_H


#include <WeexCore/WeexJSServer/task/WeexTask.h>
#include <WeexCore/WeexJSServer/object/args/InitFrameworkArgs.h>

class InitFrameworkTask : public WeexTask {
public:


    explicit InitFrameworkTask(const String &instanceId, const String &script,
                               std::vector<INIT_FRAMEWORK_PARAMS *> &params);

    explicit InitFrameworkTask(const String &script, std::vector<INIT_FRAMEWORK_PARAMS *> &params);


    explicit InitFrameworkTask(const String &instanceId, const String &script, IPCArguments *arguments,
                               size_t startCount);

    explicit InitFrameworkTask(const String &script, IPCArguments *arguments, size_t startCount);

    void run(WeexRuntime *runtime) override;


    ~InitFrameworkTask();

private:
    InitFrameworkArgs *args;
    String script;
};


#endif //WEEXV8_INITFRAMEWORKTASK_H
