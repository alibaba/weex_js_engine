# Toolchain config for Android NDK.
# This is expected to be used with a standalone Android toolchain (see
# docs/STANDALONE-TOOLCHAIN.html in the NDK on how to get one).
#
# Usage:
# mkdir build; cd build
# cmake ..; make
# mkdir android; cd android
# cmake -DLLVM_ANDROID_TOOLCHAIN_DIR=/path/to/android/ndk \
#   -DCMAKE_TOOLCHAIN_FILE=../../cmake/platforms/Android.cmake ../..
# make <target>
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)
SET(CMAKE_SYSTEM_NAME Linux)
SET(CMAKE_TOOLCHAIN_PATH $ENV{TOOLCHAIN_PATH})
SET(CMAKE_BINARY_PATH $ENV{BINARY_PATH})
SET(CMAKE_HOST_NAME $ENV{HOST})
SET(CMAKE_ANDROID_SYSROOT $ENV{NDK_ROOT}/platforms/android-$ENV{APILEVEL}/arch-arm)
SET(CMAKE_ANDROID_STL_INCLUDE $ENV{NDK_ROOT}/sources/cxx-stl/llvm-libc++/include)
SET(CMAKE_ANDROID_STL_BITS_INCLUDE $ENV{NDK_ROOT}/sources/cxx-stl/llvm-libc++abi/include)
SET(CMAKE_C_COMPILER ${CMAKE_TOOLCHAIN_PATH}/clang)
SET(CMAKE_CXX_COMPILER ${CMAKE_TOOLCHAIN_PATH}/clang++)
SET(CMAKE_FIND_ROOT_PATH ${CMAKE_BINARY_PATH})
SET(CMAKE_AR ${CMAKE_BINARY_PATH}/../../bin/arm-linux-androideabi-gcc-ar)
SET(CMAKE_RANLIB ${CMAKE_BINARY_PATH}/ranlib)

SET(ANDROID "1" CACHE STRING "ANDROID" FORCE)

SET(ANDROID_COMMON_FLAGS "-target armv7-linux-android -mthumb --sysroot=${CMAKE_ANDROID_SYSROOT} -isystem ${CMAKE_ANDROID_STL_INCLUDE} -isystem ${CMAKE_ANDROID_STL_BITS_INCLUDE} -B${CMAKE_BINARY_PATH} -ffunction-sections -fdata-sections")
SET(CMAKE_C_FLAGS "${ANDROID_COMMON_FLAGS}" CACHE STRING "toolchain_cflags" FORCE)
SET(CMAKE_CXX_FLAGS "${ANDROID_COMMON_FLAGS} -std=gnu++1y" CACHE STRING "toolchain_cxxflags" FORCE)
SET(CMAKE_EXE_LINKER_FLAGS "-pie" CACHE STRING "toolchain_exelinkflags" FORCE)
SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--gc-sections -Wl,--build-id=sha1" CACHE STRING "toolchain_exelinkflags" FORCE)
SET(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "-shared")
SET(CMAKE_SHARED_LIBRARY_PREFIX "lib")
SET(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
SET(CMAKE_SHARED_LIBRARY_SONAME_C_FLAG "")
link_libraries("-lc++_static -lc++abi -lunwind -landroid_support")
link_directories(
$ENV{NDK_ROOT}/toolchains/arm-linux-androideabi-4.9/prebuilt/${CMAKE_HOST_NAME}/lib/gcc/arm-linux-androideabi/4.9.x/armv7-a/thumb/
$ENV{NDK_ROOT}/toolchains/arm-linux-androideabi-4.9/prebuilt/${CMAKE_HOST_NAME}/arm-linux-androideabi/lib/armv7-a/thumb/
$ENV{NDK_ROOT}/sources/cxx-stl/llvm-libc++/libs/armeabi-v7a
)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
