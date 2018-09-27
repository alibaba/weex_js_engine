//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_CREATEAPPCONTEXTTASK_H
#define WEEXV8_CREATEAPPCONTEXTTASK_H


#include <WeexCore/WeexJSServer/task/WeexTask.h>

class CreateAppContextTask : public WeexTask {
public:
    CreateAppContextTask(const String &instanceId, const String &script);

    void run(WeexRuntime *runtime) override;
    std::string taskName() override { return " CreateAppContextTask "; }

private:
    String script;
};


#endif //WEEXV8_CREATEAPPCONTEXTTASK_H
