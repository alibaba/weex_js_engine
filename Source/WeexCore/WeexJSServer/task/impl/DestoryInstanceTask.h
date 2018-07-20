//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_DESTORYINSTANCETASK_H
#define WEEXV8_DESTORYINSTANCETASK_H


#include <WeexCore/WeexJSServer/task/WeexTask.h>

class DestoryInstanceTask :public  WeexTask {
public:
    DestoryInstanceTask(const String &instanceId);


    void run(WeexRuntime *runtime) override;
};


#endif //WEEXV8_DESTORYINSTANCETASK_H
