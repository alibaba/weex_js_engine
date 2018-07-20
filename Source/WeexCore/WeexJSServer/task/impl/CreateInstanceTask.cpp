//
// Created by Darin on 20/07/2018.
//

#include "CreateInstanceTask.h"


void CreateInstanceTask::addExtraArg(String arg) {
    extraArgs.push_back(arg);
}

CreateInstanceTask::CreateInstanceTask(const String &instanceId, const String &script) : WeexTask(instanceId) {
    this->script = script;
}

void CreateInstanceTask::run(WeexRuntime *runtime) {
    if (extraArgs.size() < 4)
        return;

    runtime->createInstance(instanceId, extraArgs.at(0), this->script, extraArgs.at(1), extraArgs.at(2),
                            extraArgs.at(3));

}
