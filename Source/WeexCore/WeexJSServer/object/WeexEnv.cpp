//
// Created by Darin on 2018/7/22.
//

#include "WeexEnv.h"

WeexEnv *WeexEnv::env_ = nullptr;

WeexIPCClient *WeexEnv::ipcClient() { return ipcClient_.get(); }

void WeexEnv::setIpcClient(WeexIPCClient *ipcClient) {
    this->ipcClient_.reset(ipcClient);
}

WeexJSServer *WeexEnv::ipcServer() { return ipcServer_.get(); }

TimerQueue *WeexEnv::timerQueue() { return weexTimerQueue_.get(); }

void WeexEnv::setTimerQueue(TimerQueue *timerQueue) { weexTimerQueue_.reset(timerQueue); }

WeexCore::ScriptBridge *WeexEnv::scriptBridge() { return scriptBridge_; }

void WeexEnv::setIpcServer(WeexJSServer *ipcServer) {
    ipcServer_.reset(ipcServer);
}

bool WeexEnv::multiProcess() { return isMultiProcess; }

void WeexEnv::setMultiProcess(bool multiProcess) { isMultiProcess = multiProcess; }

bool WeexEnv::useWson() { return isUsingWson; }

void WeexEnv::setUseWson(bool useWson) { isUsingWson = useWson; }

void WeexEnv::setScriptBridge(WeexCore::ScriptBridge *scriptBridge) { scriptBridge_ = scriptBridge; }

