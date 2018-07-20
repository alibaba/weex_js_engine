//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_UPDATEGLOBALCONFIGTASK_H
#define WEEXV8_UPDATEGLOBALCONFIGTASK_H


#include <WeexCore/WeexJSServer/task/WeexTask.h>

class UpdateGlobalConfigTask: public WeexTask {
public:
    UpdateGlobalConfigTask(const String &script);

    void run(WeexRuntime *runtime) override;

private:
    String script;

};


#endif //WEEXV8_UPDATEGLOBALCONFIGTASK_H
