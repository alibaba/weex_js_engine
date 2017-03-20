cmake_minimum_required(VERSION 2.8.12)

add_subdirectory(icu)
add_subdirectory(WTF)
add_subdirectory(JavaScriptCore)

include(WebKitCommon)

set(weexv8_SOURCES
weexjsc.cpp
Trace.cpp
)

set(weexv8_LIBRARIES
JavaScriptCore
log)

WEBKIT_FRAMEWORK(weexv8)


set_target_properties(weexv8
PROPERTIES
LINK_FLAGS "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}//version_script.txt -Wl,--no-undefined"
)
