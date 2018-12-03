#!/bin/bash
./build.jsc.sh

SDK_PATH=/Volumes/Data/code/work/weex/sdk/weexsdk/gitlab/android

cp *.so ${SDK_PATH}/sdk/libs/armeabi
cp *.so ${SDK_PATH}/sdk/libs/armeabi-v7a

#cd ${SDK_PATH}

#./gradlew assembleDebug


#adb uninstall com.alibaba.weex && adb install .//playground/app/build/outputs/apk/playground-debug.apk


