# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

# libweexcore
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := $(JNI_H_INCLUDE) \
		            $(LOCAL_PATH)/v8core \
					$(LOCAL_PATH)/v8core/v8/include
			
LOCAL_MODULE    := weexcore

v8_CPP_LIST += $(wildcard $(LOCAL_PATH)/v8core/*.cpp)


LOCAL_SRC_FILES := $(v8_CPP_LIST:$(LOCAL_PATH)/%=%)
LOCAL_CFLAGS += -ffunction-sections -fdata-sections


LOCAL_LDFLAGS := -Wl,--allow-multiple-definition \
                 -Wl,--version-script=$(LOCAL_PATH)/version_script.txt \
				 -Wl,--gc-sections


ifeq ($(TARGET_ARCH_ABI), x86)
    ARCH := ia32
else
    ARCH := arm
endif

v8basepath := $(LOCAL_PATH)/v8core/v8
v8libpath := $(v8basepath)/out/android_$(ARCH).release//obj.target/tools/gyp/
v8libs := $(v8libpath)/libv8_base.a $(v8libpath)/libpreparser_lib.a $(v8libpath)/libv8_nosnapshot.a

$(v8libs): MY_ARCH_ABI := $(TARGET_ARCH_ABI)

$(v8libs):
	cd $(v8basepath) && TARGET_ARCH=$(MY_ARCH_ABI) ./build.sh

$(TARGET_OUT)/libweexcore.so: $(v8libs)

LOCAL_LDLIBS := -llog -L$(v8libpath) -lv8_base -lpreparser_lib -lv8_nosnapshot

include $(BUILD_SHARED_LIBRARY)
