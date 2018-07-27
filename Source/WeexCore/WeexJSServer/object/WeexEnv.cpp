//
// Created by Darin on 2018/7/22.
//

#include "WeexEnv.h"

WeexEnv *WeexEnv::env_ = nullptr;

TimerQueue *WeexEnv::timerQueue() { return weexTimerQueue_.get(); }

void WeexEnv::setTimerQueue(TimerQueue *timerQueue) { weexTimerQueue_.reset(timerQueue); }

WeexCore::ScriptBridge *WeexEnv::scriptBridge() { return scriptBridge_; }


bool WeexEnv::useWson() { return isUsingWson; }

void WeexEnv::setUseWson(bool useWson) { isUsingWson = useWson; }

void WeexEnv::setScriptBridge(WeexCore::ScriptBridge *scriptBridge) { scriptBridge_ = scriptBridge; }

