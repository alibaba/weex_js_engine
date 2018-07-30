//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_EXEJSTASK_H
#define WEEXV8_EXEJSTASK_H


#include <WeexCore/WeexJSServer/task/WeexTask.h>
#include <WeexCore/WeexJSServer/object/args/ExeJsArgs.h>

class ExeJsTask : public WeexTask {
public:
    ExeJsTask(const String &instanceId, std::vector<VALUE_WITH_TYPE *> &params, bool withResult = false);

    ExeJsTask(const String &instanceId, IPCArguments *arguments, size_t startCount, bool withResult = false);


    void addExtraArg(String arg);


    void run(WeexRuntime *runtime) override;


    ~ExeJsTask() override;

private:

    bool withResult;
    ExeJsArgs *exeJsArgs;
    std::vector<String> extraArgs;
};


#endif //WEEXV8_EXEJSTASK_H
