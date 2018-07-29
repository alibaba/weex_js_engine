//
// Created by Darin on 23/05/2018.
//

#include <WeexCore/WeexJSServer/task/impl/NativeTimerTask.h>
#include "WeexTaskQueue.h"
#include <WeexCore/WeexJSServer/object/WeexEnv.h>
#include <unistd.h>
#include <WeexCore/WeexJSServer/bridge/script/script_bridge_in_multi_process.h>
#include <WeexCore/WeexJSServer/bridge/script/core_side_in_multi_process.h>
#include <WeexCore/WeexJSServer/bridge/platform/platform_bridge_in_multi_process.h>
#include <WeexCore/WeexJSServer/bridge/platform/platform_side_multi_process.h>

void WeexTaskQueue::run(WeexTask *task) {
    if (this->weexRuntime == nullptr) {
        LOGE("WeexCore init runtime %d fd1 = %d, fd2 = %d", gettid(),WeexEnv::getEnv()->getIpcServerFd(),
             WeexEnv::getEnv()->getIpcClientFd());
        this->weexRuntime = new WeexRuntime(WeexEnv::getEnv()->scriptBridge(), this->isMultiProgress);
        // init IpcClient in Js Thread
        if (this->isMultiProgress) {
            auto *client = new WeexIPCClient(WeexEnv::getEnv()->getIpcClientFd());
            static_cast<weex::bridge::js::CoreSideInMultiProcess *>(weex::bridge::js::ScriptBridgeInMultiProcess::Instance()->core_side())->set_ipc_client(
                    client);
            static_cast<weex::PlatformSideInMultiProcess *>(weex::PlatformBridgeInMultiProcess::Instance()->platform_side())->set_client(
                    client);
        }

    }
    WeexEnv::getEnv()->setTimerQueue(new TimerQueue(this));
    task->run(weexRuntime);
    delete task;
}


WeexTaskQueue::~WeexTaskQueue() {
    delete this->weexRuntime;
}

int WeexTaskQueue::addTask(WeexTask *task) {
    return _addTask(task, false);
}


WeexTask *WeexTaskQueue::getTask() {
    WeexTask *task = nullptr;
    while (task == nullptr) {
        threadLocker.lock();
        while (taskQueue_.empty() || !isInitOk) {
            threadLocker.wait();
        }

        if (taskQueue_.empty()) {
            threadLocker.unlock();
            continue;
        }

        assert(!taskQueue_.empty());
        task = taskQueue_.front();
        taskQueue_.pop_front();
        threadLocker.unlock();
    }

    return task;
}

int WeexTaskQueue::addTimerTask(String id, JSC::JSValue function) {
    WeexTask *task = new NativeTimerTask(std::move(id), function);
    return _addTask(
            task,
            true);
}

void WeexTaskQueue::removeTimer(int id) {
//todo 是否需要将任务队列里的 timer 任务删掉..
}

void WeexTaskQueue::start() {
    while (true) {
        auto pTask = getTask();
        if (pTask == nullptr)
            continue;
        run(pTask);
    }
}


static void *startThread(void *td) {
    auto *self = static_cast<WeexTaskQueue *>(td);
    self->isInitOk = true;
    auto pTask = self->getTask();
    LOGE("start weex queue task thread");
    self->run(pTask);
    self->start();
}

void WeexTaskQueue::init() {
    pthread_t thread;
    LOGE("start weex queue init");
    pthread_create(&thread, nullptr, startThread, this);
}

int WeexTaskQueue::_addTask(WeexTask *task, bool front) {
    threadLocker.lock();
    if (front) {
        taskQueue_.push_front(std::move(task));
    } else {
        taskQueue_.push_back(std::move(task));
    }

    int size = taskQueue_.size();
    threadLocker.unlock();
    threadLocker.signal();
    return size;
}

WeexTaskQueue::WeexTaskQueue(bool isMultiProgress) {
    this->isMultiProgress = isMultiProgress;
    this->weexRuntime = nullptr;
}

