//
// Created by Darin on 20/07/2018.
//

#ifndef WEEXV8_CTIMECALLBACKASK_H
#define WEEXV8_CTIMECALLBACKASK_H


#include <WeexCore/WeexJSServer/task/WeexTask.h>

class CTimeCallBackask :public  WeexTask{
public:
    CTimeCallBackask(const String &script);

    void run(WeexRuntime *runtime) override;

private:
    String script;

};


#endif //WEEXV8_CTIMECALLBACKASK_H
