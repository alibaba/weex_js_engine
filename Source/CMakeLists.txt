cmake_minimum_required(VERSION 2.8.12)

add_definitions(-DPRINT_LOG_CACHEFILE=0)
add_definitions(-DOS_ANDROID)

if (NOT USE_SYSTEM_MALLOC)
    add_subdirectory(bmalloc)
endif ()
add_subdirectory(WTF)
add_subdirectory(JavaScriptCore)
add_subdirectory(CrashHandler)
#include_directories(/Users/yxp/Library/Android/sdk/ndk-bundle/platforms/android-24/arch-arm/usr/include)
#include_directories(/Volumes/Data/env/ndk/android-ndk-r13b/platforms/android-24/arch-arm/usr/include)
#include_directories(/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/v1)

#add_executable(TestWeexServerPolicy
#TestWeexServerPolicy.cpp
#Compatible.cpp
#)

#add_executable(TestCrashHandler
#TestCrashHandler.cpp
#)
#
#target_link_libraries(TestCrashHandler
#CrashHandler)

include_directories(include)
include(WebKitCommon)

set(WEEXJSSERVER_NAME weexjss${SERVERSUFFIX})
#set(RENDER_PATH /Users/darin/Data/code/work/weex/sdk/gitlab/weex_core/Source)
set(RENDER_PATH /Volumes/Data/code/work/weex/sdk/weexsdk/gitlab/weex_core/Source)

include_directories(${RENDER_PATH})
# IPC
set(IPC_SOURCE_PATH ${RENDER_PATH}/IPC)
include_directories(${IPC_SOURCE_PATH})
set(IPC_SOURCES
        ${IPC_SOURCE_PATH}/Serializing/IPCSerializer.cpp
#        ${IPC_SOURCE_PATH}/Android/IPCStringResult.cpp
        ${IPC_SOURCE_PATH}/IPCResult.cpp
        ${IPC_SOURCE_PATH}/IPCSender.cpp
        ${IPC_SOURCE_PATH}/IPCException.cpp
        ${IPC_SOURCE_PATH}/IPCCommunicator.cpp
        ${IPC_SOURCE_PATH}/IPCHandler.cpp
        ${IPC_SOURCE_PATH}/IPCListener.cpp
        ${IPC_SOURCE_PATH}/IPCFutexPageQueue.cpp
        ${IPC_SOURCE_PATH}/IPCFutexPageQueue.cpp
        ${IPC_SOURCE_PATH}/ashmem.c
        ${IPC_SOURCE_PATH}/IPCCheck.cpp
        )

set(${WEEXJSSERVER_NAME}_SOURCES
        ./base/base64/base64.cpp
        WeexCore/WeexJSServer/utils/LogUtils.cpp
        ./base/base64/modp_base64/modp_b64.cc
        WeexCore/WeexJSServer/wson/wsonjsc.cpp
        WeexCore/WeexJSServer/utils/Utils.cpp
        WeexCore/WeexJSServer/utils/Compatible.cpp
        WeexCore/WeexJSServer/utils/WeexRuntime.cpp
        WeexCore/WeexJSServer/object/Args.cpp
        WeexCore/WeexJSServer/object/SimpleObject.cpp
        WeexCore/WeexJSServer/object/WeexGlobalObject.cpp
        WeexCore/WeexJSServer/object/WeexObjectHolder.cpp
        WeexCore/WeexJSServer/bridge/script/script_bridge_in_multi_so.cpp
        WeexCore/WeexJSServer/bridge/script/script_bridge_in_multi_process.cpp
        WeexCore/WeexJSServer/bridge/script/core_side_in_multi_so.cpp
        WeexCore/WeexJSServer/bridge/script/core_side_in_multi_process.cpp
        WeexCore/WeexJSServer/bridge/script/script_side_in_simple.cpp
        WeexCore/WeexJSServer/bridge/script/script_side_in_queue.cpp
        WeexCore/WeexJSServer/ipc/ipc_server.cpp

        WeexCore/WeexJSServer/object/WeexIPCClient.cpp
        WeexCore/WeexJSServer/utils/ThreadLocker.cpp
        WeexCore/WeexJSServer/object/args/ExeJsArgs.cpp
        WeexCore/WeexJSServer/object/args/InitFrameworkArgs.cpp
        WeexCore/WeexJSServer/object/WeexEnv.cpp

        WeexCore/WeexJSServer/task/impl/InitFrameworkTask.cpp
        WeexCore/WeexJSServer/task/impl/CreateAppContextTask.cpp
        WeexCore/WeexJSServer/task/impl/CreateInstanceTask.cpp
        WeexCore/WeexJSServer/task/impl/CallJsOnAppContextTask.cpp
        WeexCore/WeexJSServer/task/impl/DestoryAppContextTask.cpp
        WeexCore/WeexJSServer/task/impl/DestoryInstanceTask.cpp
        WeexCore/WeexJSServer/task/impl/ExeJsOnAppWithResultTask.cpp
        WeexCore/WeexJSServer/task/impl/UpdateGlobalConfigTask.cpp
        WeexCore/WeexJSServer/task/impl/CTimeCallBackask.cpp
        WeexCore/WeexJSServer/task/impl/ExeJsServicesTask.cpp
        WeexCore/WeexJSServer/task/impl/ExeJsOnInstanceTask.cpp
        WeexCore/WeexJSServer/task/impl/ExeJsTask.cpp
        WeexCore/WeexJSServer/task/impl/TakeHeapSnapShot.cpp
        WeexCore/WeexJSServer/task/impl/NativeTimerTask.cpp

        WeexCore/WeexJSServer/task/WeexTask.cpp
        WeexCore/WeexJSServer/task/WeexTaskQueue.cpp
        WeexCore/WeexJSServer/task/TimerTask.cpp
        WeexCore/WeexJSServer/task/TimerQueue.cpp


#        WeexCore/WeexJSServer/bridge/platform/platform_bridge_in_multi_so.cpp
#        WeexCore/WeexJSServer/bridge/platform/core_side_in_simple.cpp
#        WeexCore/WeexJSServer/bridge/platform/platform_side_in_multi_so.cpp
#        WeexCore/WeexJSServer/bridge/platform/platform_side_multi_process.cpp
#        WeexCore/WeexJSServer/bridge/platform/platform_bridge_in_multi_process.cpp
#        WeexCore/WeexJSServer/bridge/script/core_side_in_simple.cpp
#        WeexCore/WeexJSServer/bridge/script/script_bridge_in_simple.cpp
#
#        ${RENDER_PATH}/base/time_point.cc
#        ${RENDER_PATH}/base/thread/thread_impl_posix.cc
#        ${RENDER_PATH}/base/thread/thread_impl_android.cc
#        ${RENDER_PATH}/base/message_loop/message_loop.cc
#        ${RENDER_PATH}/base/message_loop/message_pump_posix.cc
#        ${RENDER_PATH}/base/message_loop/message_pump_android.cc
#        ${RENDER_PATH}/core/render/manager/render_manager.cpp
#        ${RENDER_PATH}/core/render/page/render_page.cpp
#        ${RENDER_PATH}/core/render/node/render_object.cpp
#        ${RENDER_PATH}/core/render/node/factory/render_creator.cpp
#        ${RENDER_PATH}/core/render/node/render_appbar.cpp
#        ${RENDER_PATH}/core/render/node/render_list.cpp
#        ${RENDER_PATH}/core/render/node/render_mask.cpp
#        ${RENDER_PATH}/core/render/node/render_scroller.cpp
#        ${RENDER_PATH}/core/render/node/render_text.cpp
#        ${RENDER_PATH}/core/render/action/render_action_add_event.cpp
#        ${RENDER_PATH}/core/render/action/render_action_remove_event.cpp
#        ${RENDER_PATH}/core/render/action/render_action_add_element.cpp
#        ${RENDER_PATH}/core/render/action/render_action_remove_element.cpp
#        ${RENDER_PATH}/core/render/action/render_action_move_element.cpp
#        ${RENDER_PATH}/core/render/action/render_action_createbody.cpp
#        ${RENDER_PATH}/core/render/action/render_action_createfinish.cpp
#        ${RENDER_PATH}/core/render/action/render_action_render_success.cpp
#        ${RENDER_PATH}/core/render/action/render_action_appendtree_createfinish.cpp
#        ${RENDER_PATH}/core/render/action/render_action_layout.cpp
#        ${RENDER_PATH}/core/render/action/render_action_update_attr.cpp
#        ${RENDER_PATH}/core/render/action/render_action_update_style.cpp
#
#        ${RENDER_PATH}/core/layout/layout.cpp
#        ${RENDER_PATH}/core/layout/style.cpp
#        ${RENDER_PATH}/core/css/css_value_getter.cpp
#
#        ${RENDER_PATH}/core/config/core_environment.cpp
#
#        ${RENDER_PATH}/core/moniter/render_performance.cpp
#        ${RENDER_PATH}/core/manager/weex_core_manager.cpp
#        ${RENDER_PATH}/core/parser/dom_wson.cpp
#        ${RENDER_PATH}/wson/wson_parser.cpp
#        ${RENDER_PATH}/wson/wson_util.cpp
        ${RENDER_PATH}/wson/wson.c
#
#        ${RENDER_PATH}/android/base/jni/android_jni.cpp
#        ${RENDER_PATH}/android/base/jni/jbytearray_ref.cpp
#        ${RENDER_PATH}/android/base/jni/scoped_java_ref.cpp
#        ${RENDER_PATH}/android/base/string/scoped_jstring.cpp
#        ${RENDER_PATH}/android/base/string/scoped_jstring_utf8.cpp
#        ${RENDER_PATH}/android/base/string/jstring_cache.cpp
#
#        ${RENDER_PATH}/android/bridge/impl/measure_mode_impl_android.cpp
#        ${RENDER_PATH}/android/bridge/impl/content_box_measurement_impl_android.cpp

        ${IPC_SOURCES}
)

set(${WEEXJSSERVER_NAME}_LIBRARY_TYPE SHARED)

set(${WEEXJSSERVER_NAME}_LIBRARIES
JavaScriptCore
CrashHandler
log)

WEBKIT_FRAMEWORK(${WEEXJSSERVER_NAME})


set_target_properties(${WEEXJSSERVER_NAME}
PROPERTIES
LINK_FLAGS "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}//weexjsserver_version_script.txt -Wl,--no-undefined -Wl,-soname,libweexjss${SERVERSUFFIX}.so"
)

add_custom_command(
    TARGET ${WEEXJSSERVER_NAME}
    POST_BUILD
    COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${WEEXJSSERVER_NAME}> -o ${CMAKE_SOURCE_DIR}/libweexjss${SERVERSUFFIX}.so
    VERBATIM
)

set(WEEXJSSERVERSTUB_NAME weexjsstub${SERVERSUFFIX})
add_executable(${WEEXJSSERVERSTUB_NAME}
WeexJSServerMainStub.cpp)

target_link_libraries(${WEEXJSSERVERSTUB_NAME}
${WEEXJSSERVER_NAME}
)

if (APPLE)
set_target_properties(${WEEXJSSERVERSTUB_NAME}
PROPERTIES LINK_FLAGS
"-Wl,--entry=_start"
)
endif()

add_custom_command(
    TARGET ${WEEXJSSERVERSTUB_NAME}
    POST_BUILD
    COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${WEEXJSSERVERSTUB_NAME}> -o ${CMAKE_SOURCE_DIR}/libweexjsb${SERVERSUFFIX}.so
    VERBATIM
)
