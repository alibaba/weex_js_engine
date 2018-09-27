//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_TAKEHEAPSNAPSHOT_H
#define WEEXV8_TAKEHEAPSNAPSHOT_H


#include <WeexCore/WeexJSServer/task/WeexTask.h>

class TakeHeapSnapShot : public WeexTask {
public:
    TakeHeapSnapShot(const String &instanceId);

    void run(WeexRuntime *runtime) override ;
    std::string taskName() override { return "TakeHeapSnapShot"; }

};


#endif //WEEXV8_TAKEHEAPSNAPSHOT_H
