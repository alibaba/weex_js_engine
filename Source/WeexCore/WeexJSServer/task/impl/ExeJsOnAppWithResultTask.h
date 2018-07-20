//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_EXEJSONAPPWITHRESULTTASK_H
#define WEEXV8_EXEJSONAPPWITHRESULTTASK_H


#include <WeexCore/WeexJSServer/task/WeexTask.h>

class ExeJsOnAppWithResultTask : public WeexTask {
public:
    ExeJsOnAppWithResultTask(const String &instanceId, const String &script);


    void run(WeexRuntime *runtime) override;

private:
    String script;

};


#endif //WEEXV8_EXEJSONAPPWITHRESULTTASK_H
