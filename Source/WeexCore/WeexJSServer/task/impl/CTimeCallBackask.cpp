//
// Created by Darin on 20/07/2018.
//

#include "CTimeCallBackask.h"

CTimeCallBackask::CTimeCallBackask(const String &script) : WeexTask("") {}

void CTimeCallBackask::run(WeexRuntime *runtime) {
    runtime->exeCTimeCallback(script);
}
