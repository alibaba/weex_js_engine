//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_DESTORYAPPCONTEXTTASK_H
#define WEEXV8_DESTORYAPPCONTEXTTASK_H


#include <WeexCore/WeexJSServer/task/WeexTask.h>

class DestoryAppContextTask: public WeexTask {
public:
    DestoryAppContextTask(const String &instanceId);

    void run(WeexRuntime *runtime) override;
    std::string taskName() override { return "DestoryAppContextTask"; }

};


#endif //WEEXV8_DESTORYAPPCONTEXTTASK_H
