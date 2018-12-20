//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_CREATEINSTANCETASK_H
#define WEEXV8_CREATEINSTANCETASK_H


#include <WeexCore/WeexJSServer/task/WeexTask.h>
#include <WeexCore/WeexJSServer/object/args/InitFrameworkArgs.h>

class CreateInstanceTask : public WeexTask {
public:
    CreateInstanceTask(const String &instanceId, const String &script, std::vector<INIT_FRAMEWORK_PARAMS*>& params);

    void addExtraArg(String arg);

    void run(WeexRuntime *runtime) override ;
    std::string taskName() override { return "CreateInstanceTask"; }

private:
    std::vector<String> extraArgs;
    std::unique_ptr<InitFrameworkArgs> initExtraArgs;
    String script;
};


#endif //WEEXV8_CREATEINSTANCETASK_H
