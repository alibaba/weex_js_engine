//
// Created by Darin on 20/07/2018.
//

#include "ExeJsOnAppWithResultTask.h"

ExeJsOnAppWithResultTask::ExeJsOnAppWithResultTask(const String &instanceId, const String &script) : WeexTask(
        instanceId) {
    this->script = script;
}

void ExeJsOnAppWithResultTask::run(WeexRuntime *runtime) {
    auto *jsResult = (WeexJSResult *) malloc(sizeof(WeexJSResult));
    char *string = runtime->exeJSOnAppWithResult(instanceId, script);
    jsResult->data = string;
    jsResult->fromNew = true;
    jsResult->fromMalloc = false;
    if (future() != nullptr) {
        future()->setResult(*jsResult);
    }
    free(jsResult);
}
