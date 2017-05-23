cmake_minimum_required(VERSION 2.8.12)

add_subdirectory(WTF)
add_subdirectory(JavaScriptCore)
add_subdirectory(IPC)

include(WebKitCommon)

if (${CMAKE_TARGET_WORD_BITS} EQUAL 32)
    set(SERVERSUFFIX "")
    set(weexjsc_SOURCES
    Compatible.cpp
    WeexJSConnection.cpp
    weexproxy.cpp
    )

    set(weexjsc_LIBRARY_TYPE SHARED)

    set(weexjsc_LIBRARIES
    IPC
    WTF
    log)

    WEBKIT_FRAMEWORK(weexjsc)


    set_target_properties(weexjsc
    PROPERTIES
    LINK_FLAGS "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}//weexjsc_version_script.txt -Wl,--no-undefined -Wl,-soname,libweexjsc.so"
    )

    add_custom_command(
        TARGET weexjsc
        POST_BUILD
        COMMAND ${CMAKE_STRIP} $<TARGET_FILE:weexjsc> -o ${CMAKE_SOURCE_DIR}/libweexjsc.so
        VERBATIM
    )
else()
    set(SERVERSUFFIX 64)
endif()

set(WEEXJSSERVER_NAME weexjsserver${SERVERSUFFIX})
set(${WEEXJSSERVER_NAME}_SOURCES
Compatible.cpp
WeexJSServer.cpp
WeexJSServerMain.cpp
)

set(${WEEXJSSERVER_NAME}_LIBRARY_TYPE SHARED)

set(${WEEXJSSERVER_NAME}_LIBRARIES
JavaScriptCore
IPC
log)

WEBKIT_FRAMEWORK(${WEEXJSSERVER_NAME})


set_target_properties(${WEEXJSSERVER_NAME}
PROPERTIES
LINK_FLAGS "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}//weexjsserver_version_script.txt -Wl,--no-undefined -Wl,-soname,libweexjsserver${SERVERSUFFIX}.so"
)

add_custom_command(
    TARGET ${WEEXJSSERVER_NAME}
    POST_BUILD
    COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${WEEXJSSERVER_NAME}> -o ${CMAKE_SOURCE_DIR}/libweexjsserver${SERVERSUFFIX}.so
    VERBATIM
)

set(WEEXJSSERVERSTUB_NAME weexjsserverstub${SERVERSUFFIX})
add_executable(${WEEXJSSERVERSTUB_NAME}
WeexJSServerMainStub.cpp
)

target_link_libraries(${WEEXJSSERVERSTUB_NAME}
${WEEXJSSERVER_NAME}
)

add_custom_command(
    TARGET ${WEEXJSSERVERSTUB_NAME}
    POST_BUILD
    COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${WEEXJSSERVERSTUB_NAME}> -o ${CMAKE_SOURCE_DIR}/libweexjsserverstub${SERVERSUFFIX}.so
    VERBATIM
)
