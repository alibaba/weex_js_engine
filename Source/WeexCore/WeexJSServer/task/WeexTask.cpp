//
// Created by Darin on 20/07/2018.
//

#include "WeexTask.h"

void WeexTask::Future::setResult(std::unique_ptr<WeexJSResult> &result) {
    thread_locker_.lock();
    has_result_ = true;
    result_ = std::move(result);
    thread_locker_.unlock();
    thread_locker_.signal();
}

std::unique_ptr<WeexJSResult>  WeexTask::Future::waitResult() {
    thread_locker_.lock();
    while (!has_result_) {
        thread_locker_.wait();
    }
    thread_locker_.unlock();
    return std::move(result_);
}
