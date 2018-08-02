//
// Created by Darin on 20/07/2018.
//

#include "ExeJsTask.h"

void ExeJsTask::run(WeexRuntime *runtime) {

    LOGE("ExeJsTask is running and extraArgsSize is %d", extraArgs.size());

    if (extraArgs.size() < 2)
        return;

    if (!withResult) {
        runtime->exeJS(instanceId, extraArgs.at(0), extraArgs.at(1), exeJsArgs->params);
        return;
    }
    WeexJSResult jsResult = runtime->exeJSWithResult(instanceId, extraArgs.at(0), extraArgs.at(1),
                                                     exeJsArgs->params);
    setResult(&jsResult);
}

void ExeJsTask::addExtraArg(String arg) {
    this->extraArgs.push_back(arg);
}

ExeJsTask::ExeJsTask(const String &instanceId, std::vector<VALUE_WITH_TYPE *> &params, bool withResult) : WeexTask(
        instanceId) {
    this->withResult = withResult;
    exeJsArgs = new ExeJsArgs(params);
}

ExeJsTask::ExeJsTask(const String &instanceId, IPCArguments *arguments, size_t startCount, bool withResult) : WeexTask(
        instanceId) {
    this->withResult = withResult;
    this->exeJsArgs = new ExeJsArgs(arguments, startCount);
}

ExeJsTask::~ExeJsTask() {
    LOGE("del ExeJsTask");
    delete exeJsArgs;
}
