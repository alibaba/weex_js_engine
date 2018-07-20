//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_EXEJSSERVICESTASK_H
#define WEEXV8_EXEJSSERVICESTASK_H


#include <WeexCore/WeexJSServer/task/WeexTask.h>

class ExeJsServicesTask :public  WeexTask {
public:
    explicit ExeJsServicesTask(const String &script);

     void run(WeexRuntime *runtime) override ;


private:
    String script;

};


#endif //WEEXV8_EXEJSSERVICESTASK_H
