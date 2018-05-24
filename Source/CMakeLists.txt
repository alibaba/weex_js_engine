cmake_minimum_required(VERSION 2.8.12)

add_definitions(-DPRINT_LOG_CACHEFILE=0)

if (NOT USE_SYSTEM_MALLOC)
    add_subdirectory(bmalloc)
endif ()
add_subdirectory(WTF)
add_subdirectory(JavaScriptCore)
add_subdirectory(IPC)
add_subdirectory(CrashHandler)

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
set(${WEEXJSSERVER_NAME}_SOURCES
        ./base/base64/base64.cpp
        ./base/base64/modp_base64/modp_b64.cc
        wson/wson.c
        wson/wsonjsc.cpp
        WeexCore/WeexJSServer/utils/Utils.cpp
        WeexCore/WeexJSServer/utils/LogUtils.cpp
        WeexCore/WeexJSServer/utils/Compatible.cpp
        WeexCore/WeexJSServer/utils/WeexRuntime.cpp
        WeexCore/WeexJSServer/object/Args.cpp
        WeexCore/WeexJSServer/object/SimpleObject.cpp
        WeexCore/WeexJSServer/object/WeexGlobalObject.cpp
        WeexCore/WeexJSServer/object/WeexObjectHolder.cpp
        WeexCore/WeexJSServer/WeexJSCExternal.cpp
        WeexCore/WeexJSServer/WeexJSServerApi.cpp
        WeexJSServer.cpp
        WeexJSServerMain.cpp
)

set(${WEEXJSSERVER_NAME}_LIBRARY_TYPE SHARED)

set(${WEEXJSSERVER_NAME}_LIBRARIES
JavaScriptCore
IPC
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
