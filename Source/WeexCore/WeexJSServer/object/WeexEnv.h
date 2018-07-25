//
// Created by Darin on 2018/7/22.
//

#ifndef WEEXV8_WEEXENV_H
#define WEEXV8_WEEXENV_H


#include <WeexCore/WeexJSServer/ipc/ipc_server.h>
#include <WeexCore/WeexJSServer/task/TimerQueue.h>
#include "WeexIPCClient.h"

class WeexEnv {

public:
    static WeexEnv *env() {
        if (env_ == nullptr) {
            env_ = new WeexEnv();
            env_->setUseWson(true);
            env_->setMultiProcess(true);
        }
        return env_;
    }

    WeexIPCClient *ipcClient();

    void setIpcClient(WeexIPCClient *ipcClient);


    WeexJSServer *ipcServer();

    void setIpcServer(WeexJSServer *ipcServer);

    bool multiProcess();

    void setMultiProcess(bool multiProcess);


    bool useWson();

    void setUseWson(bool useWson);


    TimerQueue *timerQueue();

    void setTimerQueue(TimerQueue *timerQueue);


    WeexCore::ScriptBridge *scriptBridge();

    void setScriptBridge(WeexCore::ScriptBridge *scriptBridge);

    int getIpcClientFd() const {
        return ipcClientFd_;
    }

    void setIpcClientFd(int ipcClientFd_) {
        WeexEnv::ipcClientFd_ = ipcClientFd_;
    }

    int getIpcServerFd() const {
        return ipcServerFd_;
    }

    void setIpcServerFd(int ipcServerFd_) {
        WeexEnv::ipcServerFd_ = ipcServerFd_;
    }

    bool enableTrace() const {
        return enableTrace_;
    }

    void setEnableTrace(bool enableTrace_) {
        WeexEnv::enableTrace_ = enableTrace_;
    }


private:
    static WeexEnv *env_;

    volatile bool isMultiProcess = true;
    volatile bool isUsingWson = true;
    // add for multiProcess;
    std::unique_ptr<WeexIPCClient> ipcClient_;
    std::unique_ptr<WeexJSServer> ipcServer_;

    std::unique_ptr<TimerQueue> weexTimerQueue_;

    WeexCore::ScriptBridge *scriptBridge_;


    volatile int ipcClientFd_;
    volatile int ipcServerFd_;
    volatile bool enableTrace_;
};


#endif //WEEXV8_WEEXENV_H
