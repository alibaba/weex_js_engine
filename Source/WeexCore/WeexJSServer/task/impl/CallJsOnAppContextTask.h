//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_CALLJSONAPPCONTEXTTASK_H
#define WEEXV8_CALLJSONAPPCONTEXTTASK_H


#include <WeexCore/WeexJSServer/task/WeexTask.h>
#include <WeexCore/WeexJSServer/object/args/ExeJsArgs.h>

class CallJsOnAppContextTask : public WeexTask {

public:
    CallJsOnAppContextTask(const String &instanceId, const String &func, std::vector<VALUE_WITH_TYPE *> param);

    CallJsOnAppContextTask(const String &instanceId, const String &func, IPCArguments *arguments, size_t startCount);

    void run(WeexRuntime *runtime) override;


    ~CallJsOnAppContextTask() {
        delete exeJsArgs;
    }

private:
    String func;
    ExeJsArgs *exeJsArgs;
};


#endif //WEEXV8_CALLJSONAPPCONTEXTTASK_H
