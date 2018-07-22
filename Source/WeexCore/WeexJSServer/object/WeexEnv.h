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
        }
        return env_;
    }

    WeexIPCClient *ipcClient() { return ipcClient_.get(); }

    WeexJSServer *ipcServer() { return ipcServer_.get(); }

    bool multiProcess() { return isMultiProcess; };

    void setMultiProcess(bool multiProcess) { isMultiProcess = multiProcess; }


    bool useWson() { return isUsingWson; }

    void setUseWson(bool useWson) { isUsingWson = useWson; }


    TimerQueue *timerQueue() { return weexTimerQueue_.get(); }

    void setTimerQueue(TimerQueue *timerQueue) { weexTimerQueue_.reset(timerQueue); };


    WeexCore::ScriptBridge *scriptBridge() { return scriptBridge_; }

    void setScriptBridge(WeexCore::ScriptBridge *scriptBridge) { scriptBridge_ = scriptBridge; }

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

    bool isMultiProcess = true;
    bool isUsingWson = false;
    // add for multiProcess;
    std::unique_ptr<WeexIPCClient> ipcClient_;
    std::unique_ptr<WeexJSServer> ipcServer_;

    std::unique_ptr<TimerQueue> weexTimerQueue_;

    WeexCore::ScriptBridge *scriptBridge_;


    int ipcClientFd_;
    int ipcServerFd_;
    bool enableTrace_;
};


#endif //WEEXV8_WEEXENV_H
