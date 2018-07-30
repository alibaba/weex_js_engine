//
// Created by Darin on 14/06/2018.
//

#include "ThreadLocker.h"
#include "Utils.h"

int ThreadLocker::lock() {
    return pthread_mutex_lock(&mutex_);
}

int ThreadLocker::unlock() {
    return pthread_mutex_unlock(&mutex_);
}

int ThreadLocker::wait() {
    return pthread_cond_wait(&condition_, &mutex_);
}


int ThreadLocker::waitTimeout(const uint64_t microSecWhen) {
    const struct timeval timeout = {static_cast<long>(microSecWhen / MICROSEC),
                                    static_cast<int>(microSecWhen % MICROSEC)};
    return waitTimeout(&timeout);
}

int ThreadLocker::waitTimeout(const struct timeval *tv) {
    struct timespec ts{tv->tv_sec, tv->tv_usec * TIMESPCE};
    LOGE("timeout value wait time sec is %lu  %lu", tv->tv_sec, tv->tv_usec * TIMESPCE);
    return waitTimeout(&ts);
}

int ThreadLocker::waitTimeout(const struct timespec *timeout) {
    struct timeval tv;
    gettimeofday(&tv, nullptr);

    LOGE("timeout value wait time sec is %lu  %lu", timeout->tv_sec - tv.tv_sec, (timeout->tv_nsec/ TIMESPCE -tv.tv_usec)/TIMESPCE);

    return pthread_cond_timedwait(&condition_, &mutex_, timeout);
}

ThreadLocker::ThreadLocker() {
    pthread_mutex_init(&mutex_, nullptr);
    pthread_cond_init(&condition_, nullptr);
}

ThreadLocker::~ThreadLocker() {
    pthread_mutex_destroy(&mutex_);
    pthread_cond_destroy(&condition_);
}

int ThreadLocker::signal() {
    return pthread_cond_signal(&condition_);
}




