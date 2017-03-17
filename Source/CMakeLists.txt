cmake_minimum_required(VERSION 2.8.12)

add_subdirectory(icu)
add_subdirectory(WTF)
add_subdirectory(JavaScriptCore)

set(WEEX_SOURCES
weexjsc.cpp
Trace.cpp
)

add_library(weexv8 SHARED
${WEEX_SOURCES}
)

target_link_libraries(weexv8
JavaScriptCore
log
)

set_target_properties(weexv8
PROPERTIES
LINK_FLAGS "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}//version_script.txt -Wl,--no-undefined"
)
