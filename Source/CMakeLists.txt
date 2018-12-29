cmake_minimum_required(VERSION 3.3)
if (POLICY CMP0042)
    cmake_policy(SET CMP0042 NEW)
endif ()

include(WeexCommon)

add_definitions(-DPRINT_LOG_CACHEFILE=0)
add_definitions(-DOS_ANDROID=1)
add_definitions(-DWTF_OS_ANDROID=1)

if ($ENV{APILEVEL} LESS 21)
    add_definitions(-D_POSIX_THREAD_KEYS_MAX=128)
    add_definitions(-DPTHREAD_KEYS_MAX=_POSIX_THREAD_KEYS_MAX)
endif ()

set(USE_SYSTEM_MALLOC 1)

add_definitions(-DUSE_SYSTEM_MALLOC=1)
add_definitions(-DUSE_EXPORT_MACROS=1)
add_definitions(-DOS_ANDROID=1)

if (NOT USE_SYSTEM_MALLOC)
    add_subdirectory(bmalloc)
endif ()
add_subdirectory(WTF)
add_subdirectory(JavaScriptCore)

include_directories(include)

link_directories(${CMAKE_SOURCE_DIR})
