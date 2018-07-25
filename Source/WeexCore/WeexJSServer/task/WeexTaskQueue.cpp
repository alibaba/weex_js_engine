//
// Created by Darin on 23/05/2018.
//

#include <WeexCore/WeexJSServer/task/impl/NativeTimerTask.h>
#include "WeexTaskQueue.h"
#include <WeexCore/WeexJSServer/object/WeexEnv.h>
#include <unistd.h>
void WeexTaskQueue::run(WeexTask *task) {
    if (this->weexRuntime == nullptr) {
        LOGE("WeexCore init runtime %d fd1 = %d, fd2 = %d",gettid(),WeexEnv::env()->getIpcServerFd(),WeexEnv::env()->getIpcClientFd());
        this->weexRuntime = new WeexRuntime(WeexEnv::env()->scriptBridge(), true);
        // init IpcClient in Js Thread
        auto *client = new WeexIPCClient(WeexEnv::env()->getIpcClientFd());
        WeexEnv::env()->setIpcClient(client);
    }
    TimerQueue *queue = new TimerQueue(this);
    queue->init();
    WeexEnv::env()->setTimerQueue(queue);
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
//        pthread_mutex_lock(&mutex_);
        threadLocker.lock();
        while (taskQueue_.empty() || !isInitOk) {
//            pthread_cond_wait(&condition_, &mutex_);
            threadLocker.wait();
        }

        if (taskQueue_.empty()) {
//            pthread_mutex_unlock(&mutex_);
            threadLocker.unlock();
            continue;
        }

        assert(!taskQueue_.empty());
        task = taskQueue_.front();
        taskQueue_.pop_front();
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
    auto pTask = self->getTask();
    self->run(pTask);
    self->start();
}

void WeexTaskQueue::init() {
    pthread_t thread;
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

