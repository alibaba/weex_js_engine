#!/bin/bash
./build.jsc.sh

SDK_PATH=/Users/darin/Documents/code/weex/sdk/weexSdk/apache/android

cp libweexjs*.so ${SDK_PATH}/sdk/libs/armeabi
cp libweexjs*.so ${SDK_PATH}/sdk/libs/armeabi-v7a

cd ${SDK_PATH}

#./gradlew assembleDebug


#adb uninstall com.alibaba.weex && adb install .//playground/app/build/outputs/apk/playground-debug.apk


