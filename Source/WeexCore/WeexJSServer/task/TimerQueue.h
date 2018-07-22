//
// Created by Darin on 12/06/2018.
//

#ifndef WEEXV8_TIMERQUEUE_H
#define WEEXV8_TIMERQUEUE_H

#include "WeexTaskQueue.h"
#include <pthread.h>
#include <deque>
#include "TimerTask.h"

class TimerTask;

class TimerQueue {

public:
    void init();

    void removeTimer(int timerId);

    int addTimerTask(TimerTask *timerTask);

    TimerTask *getTask();

    void start();

    bool isInit = false;

    explicit TimerQueue(WeexTaskQueue *taskQueue);

private:
    uint64_t nextTaskWhen;
    WeexTaskQueue *weexTaskQueue;
    std::deque<TimerTask *> timerQueue_;
    ThreadLocker threadLocker;
};


#endif //WEEXV8_TIMERQUEUE_H
