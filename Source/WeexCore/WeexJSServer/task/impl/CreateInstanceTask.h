//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_CREATEINSTANCETASK_H
#define WEEXV8_CREATEINSTANCETASK_H


#include <WeexCore/WeexJSServer/task/WeexTask.h>

class CreateInstanceTask : public WeexTask {
public:
    CreateInstanceTask(const String &instanceId, const String &script);

    void addExtraArg(String arg);

    void run(WeexRuntime *runtime) override ;

private:
    std::vector<String> extraArgs;
    String script;
};


#endif //WEEXV8_CREATEINSTANCETASK_H
