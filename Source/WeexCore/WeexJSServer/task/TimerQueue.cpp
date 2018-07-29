//
// Created by Darin on 12/06/2018.
//

#include "TimerQueue.h"

static void *startThread(void *td) {
    auto *self = static_cast<TimerQueue *>(td);
    self->start();
}

void TimerQueue::init() {

    if (this->isInit)
        return;

    this->isInit = true;

    pthread_t thread;
    pthread_create(&thread, nullptr, startThread, this);
    pthread_setname_np(thread, "TimerQueueThread");
}

void TimerQueue::start() {
    while (true) {
        auto pTask = getTask();
        weexTaskQueue->addTimerTask(pTask->instanceID, pTask->function);
        if (pTask->repeat) {
            LOGE("repreat");
            addTimerTask(new TimerTask(pTask));
        }
        delete (pTask);
        pTask = nullptr;
    }
}

TimerQueue::TimerQueue(WeexTaskQueue *taskQueue) {
    nextTaskWhen = 0;
    this->weexTaskQueue = taskQueue;
    init();
}


int TimerQueue::addTimerTask(TimerTask *timerTask) {
    threadLocker.lock();
    auto size = timerQueue_.size();
    if (timerQueue_.empty()) {
        timerQueue_.push_back(timerTask);
    } else {
        auto begin = timerQueue_.begin();
        int i = 0;
        for (; i < size; ++i) {
            auto it = timerQueue_[i];
            if (timerTask->when < it->when) {
                timerQueue_.insert(begin + i, timerTask);
                break;
            } else if (i == size - 1) {
                timerQueue_.push_back(timerTask);
                break;
            }
        }
    }

    size = timerQueue_.size();
    if (size > 0) {
        nextTaskWhen = timerQueue_.front()->when;
    }
    threadLocker.unlock();
    threadLocker.signal();
    return size;
}

//对比时间
TimerTask *TimerQueue::getTask() {
    TimerTask *task = nullptr;
    while (task == nullptr) {
        threadLocker.lock();
        while (timerQueue_.empty() || microTime() < nextTaskWhen) {
            if (timerQueue_.empty()) {
                threadLocker.wait();
            } else {
                auto i = threadLocker.waitTimeout(nextTaskWhen);
                if (i == ETIMEDOUT) {
                    break;
                }
            }
        }

        if (timerQueue_.empty()) {
            threadLocker.unlock();
            continue;
        }
        assert(!taskQueue_.empty());
        TimerTask *header = timerQueue_.front();
        nextTaskWhen = header->when;
        if (microTime() > nextTaskWhen) {
            timerQueue_.pop_front();
            task = header;
        } else {
            threadLocker.unlock();
            continue;
        }
        threadLocker.unlock();
    }
    return task;
}

void TimerQueue::removeTimer(int timerId) {
    threadLocker.lock();
    if (timerQueue_.empty()) {
        threadLocker.unlock();
        return;
    } else {
        for (std::deque<TimerTask *>::iterator it = timerQueue_.begin(); it < timerQueue_.end(); ++it) {

            auto reference = *it;
            if (reference->timeId == timerId) {
                timerQueue_.erase(it);
                delete (reference);
                reference = nullptr;
            }
        }
    }

    int size = timerQueue_.size();
    if (size > 0) {
        nextTaskWhen = timerQueue_.front()->when;
    }
    //this->weexTaskQueue->removeTimer(std::move(id), std::move(timerId));
    threadLocker.unlock();
    threadLocker.signal();
}
