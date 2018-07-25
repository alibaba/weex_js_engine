//
// Created by Darin on 20/07/2018.
//

#include "InitFrameworkTask.h"

void InitFrameworkTask::run(WeexRuntime *runtime) {
    if (instanceId.isEmpty())
        runtime->initFramework(script, args->params);
    else
        runtime->initAppFramework(instanceId, script, args->params);
}

InitFrameworkTask::InitFrameworkTask(const String &script, std::vector<INIT_FRAMEWORK_PARAMS *> params)
        : InitFrameworkTask("", script, params) {

}

InitFrameworkTask::InitFrameworkTask(const String &script, IPCArguments *arguments, size_t startCount)
        : InitFrameworkTask("", script, arguments, startCount) {

}

InitFrameworkTask::InitFrameworkTask(const String &instanceId, const String &script,
                                     std::vector<INIT_FRAMEWORK_PARAMS *> params) : WeexTask(instanceId) {

    this->script = script;
    args = new InitFrameworkArgs(params);
}

InitFrameworkTask::InitFrameworkTask(const String &instanceId, const String &script, IPCArguments *arguments,
                                     size_t startCount) : WeexTask(instanceId) {
    this->script = script;
    args = new InitFrameworkArgs(arguments, startCount);
}

InitFrameworkTask::~InitFrameworkTask() {
    delete args;
}
