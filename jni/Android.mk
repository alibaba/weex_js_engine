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
		            $(LOCAL_PATH)/v8core
			
LOCAL_MODULE    := weexcore

v8_CPP_LIST += $(wildcard $(LOCAL_PATH)/v8core/*.cpp)


LOCAL_SRC_FILES := $(v8_CPP_LIST:$(LOCAL_PATH)/%=%)


LOCAL_LDFLAGS := -Wl,--allow-multiple-definition

LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog -L$(LOCAL_PATH)
ifeq ($(TARGET_ARCH),arm)
  LOCAL_LDFLAGS += -lv8_base_arm -lv8_snapshot_arm
else
  ifeq ($(TARGET_ARCH),x86)
    LOCAL_LDFLAGS += -lv8_base_x86 -lpreparser_lib_x86 -lv8_nosnapshot_x86 
  endif
endif

include $(BUILD_SHARED_LIBRARY)
