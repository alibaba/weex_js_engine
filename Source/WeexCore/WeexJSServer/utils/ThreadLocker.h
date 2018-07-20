//
// Created by Darin on 14/06/2018.
//

#ifndef WEEXV8_THREADLOCKER_H
#define WEEXV8_THREADLOCKER_H


#include <pthread.h>

class ThreadLocker {


public:
    explicit ThreadLocker();

    ~ThreadLocker();

    int lock();

    int unlock();

    int wait();

    int signal();

    int waitTimeout(const uint64_t microSecWhen);

    int waitTimeout(const struct timeval *);

    int waitTimeout(const struct timespec *);


private:
    pthread_mutex_t mutex_;
    pthread_cond_t condition_;
};


#endif //WEEXV8_THREADLOCKER_H
