//
// Created by Darin on 20/07/2018.
//

#include "ExeJsTask.h"

void ExeJsTask::run(WeexRuntime *runtime) {
    if (extraArgs.size() < 2)
        return;

    if (callbackId >= 0) {
        runtime->exeJSWithCallback(instanceId, extraArgs.at(0), extraArgs.at(1),
                                 exeJsArgs->params, callbackId);
    } else if (!withResult) {
        runtime->exeJS(instanceId, extraArgs.at(0), extraArgs.at(1), exeJsArgs->params);
    } else {
        std::unique_ptr<WeexJSResult>  jsResult = runtime->exeJSWithResult(instanceId, extraArgs.at(0), extraArgs.at(1),
                                                                           exeJsArgs->params);

        if (future() != nullptr) {
            future()->setResult(jsResult);
        }
    }
}

void ExeJsTask::addExtraArg(String arg) {
    this->extraArgs.push_back(arg);
}

ExeJsTask::ExeJsTask(const String &instanceId, std::vector<VALUE_WITH_TYPE *> &params, bool withResult) : WeexTask(
        instanceId) {
    this->withResult = withResult;
    callbackId = -1;
    exeJsArgs = new ExeJsArgs(params);
}

ExeJsTask::ExeJsTask(const String &instanceId, std::vector<VALUE_WITH_TYPE *> &params, long callback_id) : WeexTask(
        instanceId) {
    this->withResult = true;
    callbackId = callback_id;
    exeJsArgs = new ExeJsArgs(params);
}

ExeJsTask::ExeJsTask(const String &instanceId, IPCArguments *arguments, size_t startCount, bool withResult) : WeexTask(
        instanceId) {
    this->withResult = withResult;
    callbackId = -1;
    this->exeJsArgs = new ExeJsArgs(arguments, startCount);
}

ExeJsTask::~ExeJsTask() {
    delete exeJsArgs;
}
