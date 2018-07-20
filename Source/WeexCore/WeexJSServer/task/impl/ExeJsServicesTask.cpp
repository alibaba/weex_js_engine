//
// Created by Darin on 20/07/2018.
//

#include "ExeJsServicesTask.h"

ExeJsServicesTask::ExeJsServicesTask(const String &script) : WeexTask("") {
    this->script = script;
}

void ExeJsServicesTask::run(WeexRuntime *runtime) {
    runtime->exeJsService(this->script);
}
