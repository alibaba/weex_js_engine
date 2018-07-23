//
// Created by Darin on 23/05/2018.
//

#include <WeexCore/WeexJSServer/task/impl/NativeTimerTask.h>
#include "WeexTaskQueue.h"
#include <WeexCore/WeexJSServer/object/WeexEnv.h>

void WeexTaskQueue::run(WeexTask *task) {
    if (this->weexRuntime == nullptr) {
        this->weexRuntime = new WeexRuntime(WeexEnv::env()->scriptBridge(), true);
        // init IpcClient in Js Thread
        auto *client = new WeexIPCClient(WeexEnv::env()->getIpcClientFd());
        WeexEnv::env()->setIpcClient(client);
    }
    task->run(weexRuntime);
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
        LOGE("getTask is running lock");
//        pthread_mutex_lock(&mutex_);
        threadLocker.lock();
        while (taskQueue_.empty() || !isInitOk) {
            LOGE("getTask is running wait");
//            pthread_cond_wait(&condition_, &mutex_);
            threadLocker.wait();
        }

        if (taskQueue_.empty()) {
            LOGE("getTask is running empty unlock");
//            pthread_mutex_unlock(&mutex_);
            threadLocker.unlock();
            continue;
        }

        assert(!taskQueue_.empty());
        task = taskQueue_.front();
        taskQueue_.pop_front();
        LOGE("getTask is running unlock");
//        pthread_mutex_unlock(&mutex_);
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
    LOGE("startThread1");
    auto pTask = self->getTask();
    LOGE("startThread2");
    self->run(pTask);
    self->start();
}

void WeexTaskQueue::init() {
    LOGE("init");
    pthread_t thread;
    pthread_create(&thread, nullptr, startThread, this);
}

int WeexTaskQueue::_addTask(WeexTask *task, bool front) {
    LOGE("_addTask is running lock + %d %s", (int) pthread_self(), front ? "front" : "back");
    threadLocker.lock();
    if (front) {
        taskQueue_.push_front(std::move(task));
    } else {
        taskQueue_.push_back(std::move(task));
    }

    int size = taskQueue_.size();
    LOGE("_addTask is running unlock and size is %d", size);
    threadLocker.unlock();
    threadLocker.signal();
    return size;
}

