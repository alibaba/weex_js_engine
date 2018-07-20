//
// Created by Darin on 20/07/2018.
//

#include "CallJsOnAppContextTask.h"

CallJsOnAppContextTask::CallJsOnAppContextTask(const String &instanceId, const String &func,
                                               std::vector<VALUE_WITH_TYPE *> param) : WeexTask(instanceId) {

    this->func = func;

    exeJsArgs = new ExeJsArgs(param);

}

CallJsOnAppContextTask::CallJsOnAppContextTask(const String &instanceId, const String &func, IPCArguments *arguments,
                                               size_t startCount) : WeexTask(instanceId) {

    this->func = func;

    exeJsArgs = new ExeJsArgs(arguments, startCount);

}

void CallJsOnAppContextTask::run(WeexRuntime *runtime) {
    runtime->callJSOnAppContext(instanceId, func, exeJsArgs->param);
}
