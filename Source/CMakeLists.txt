cmake_minimum_required(VERSION 2.8.12)

add_subdirectory(icu)
add_subdirectory(WTF)
add_subdirectory(JavaScriptCore)

include(WebKitCommon)

set(weexjsc_SOURCES
weexjsc.cpp
Compatible.cpp
)

set(weexjsc_LIBRARY_TYPE SHARED)

set(weexjsc_LIBRARIES
JavaScriptCore
log)

WEBKIT_FRAMEWORK(weexjsc)


set_target_properties(weexjsc
PROPERTIES
LINK_FLAGS_RELWITHDEBINFO "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}//version_script.txt -Wl,--no-undefined"
)

set_target_properties(weexjsc
PROPERTIES
LINK_FLAGS_RELEASE "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}//version_script.txt -Wl,--no-undefined"
)

add_custom_command(
    TARGET weexjsc
    POST_BUILD
    COMMAND ${CMAKE_STRIP} $<TARGET_FILE:weexjsc> -o ${CMAKE_SOURCE_DIR}/libweexjsc.so
    VERBATIM
)
