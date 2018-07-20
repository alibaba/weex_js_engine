//
// Created by Darin on 20/07/2018.
//

#include "WeexTask.h"

void WeexTask::setResult(WeexJSResult *result) {
    threadLocker.lock();
    this->result = result;
    threadLocker.unlock();
    threadLocker.signal();
}

WeexJSResult *WeexTask::waitResult() {
    threadLocker.lock();
    while (this->result == nullptr) {
        threadLocker.wait();
    }
    threadLocker.unlock();
    return this->result;
}
