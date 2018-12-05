//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_CREATEINSTANCETASK_H
#define WEEXV8_CREATEINSTANCETASK_H


#include <WeexCore/WeexJSServer/task/WeexTask.h>
#include <WeexCore/WeexJSServer/object/args/ExeJsArgs.h>
class CreateInstanceTask : public WeexTask {
public:
    CreateInstanceTask(const String &instanceId, const String &script);

    void addExtraArg(String arg);

    void run(WeexRuntime *runtime) override ;
    std::string taskName() override { return "CreateInstanceTask"; }

    void addExtraOptionArgs(std::vector<VALUE_WITH_TYPE*>& params);
private:
    std::vector<String> extraArgs;
    std::unique_ptr<ExeJsArgs> exeJsArgs;
    String script;
};


#endif //WEEXV8_CREATEINSTANCETASK_H
