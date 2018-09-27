//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_EXEJSONINSTANCETASK_H
#define WEEXV8_EXEJSONINSTANCETASK_H


#include <WeexCore/WeexJSServer/task/WeexTask.h>

class ExeJsOnInstanceTask: public WeexTask {
public:
    explicit ExeJsOnInstanceTask(const String &instanceId, const String &script);

    void run(WeexRuntime *runtime) override;
    std::string taskName() override { return "ExeJsOnInstanceTask"; }
private:
    String script;
};


#endif //WEEXV8_EXEJSONINSTANCETASK_H
