cmake_minimum_required(VERSION 2.8.12)

add_library(CrashHandler
STATIC
CrashHandler.cpp)

#target_link_libraries(IPC
#JavaScriptCore)

target_include_directories(CrashHandler
PUBLIC
.)

