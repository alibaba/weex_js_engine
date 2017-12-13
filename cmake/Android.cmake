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
SET(CMAKE_TOOLCHAIN_PATH ${CMAKE_SOURCE_DIR}/arm-linux-androideabi-gcc-7.2)
SET(CMAKE_ANDROID_SYSROOT ${CMAKE_SOURCE_DIR}/arm-sysroot)
SET(CMAKE_C_COMPILER ${CMAKE_TOOLCHAIN_PATH}/bin/arm-linux-androideabi-gcc)
SET(CMAKE_CXX_COMPILER ${CMAKE_TOOLCHAIN_PATH}/bin/arm-linux-androideabi-g++)
SET(CMAKE_FIND_ROOT_PATH ${CMAKE_TOOLCHAIN_PATH})
SET(CMAKE_AR ${CMAKE_TOOLCHAIN_PATH}/bin/arm-linux-androideabi-gcc-ar)
SET(CMAKE_STRIP ${CMAKE_TOOLCHAIN_PATH}/arm-linux-androideabi/bin/strip)
SET(CMAKE_RANLIB ${CMAKE_TOOLCHAIN_PATH}/bin/arm-linux-androideabi-gcc-ranlib)

SET(ANDROID "1" CACHE STRING "ANDROID" FORCE)

SET(ANDROID_COMMON_FLAGS "${CMAKE_CLANG_CFLAGS} --sysroot=${CMAKE_ANDROID_SYSROOT} -ffunction-sections -fdata-sections -fomit-frame-pointer -march=armv7-a -mthumb -mfpu=neon -mfloat-abi=softfp")
SET(CMAKE_C_FLAGS "${ANDROID_COMMON_FLAGS}" CACHE STRING "toolchain_cflags" FORCE)
SET(CMAKE_CXX_FLAGS "${ANDROID_COMMON_FLAGS} -std=gnu++1y" CACHE STRING "toolchain_cxxflags" FORCE)
SET(CMAKE_ASM_FLAGS "${ANDROID_COMMON_FLAGS}" CACHE STRING "toolchain_asmflags" FORCE)
SET(CMAKE_EXE_LINKER_FLAGS "-pie" CACHE STRING "toolchain_exelinkflags" FORCE)
SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--gc-sections -Wl,--build-id=sha1" CACHE STRING "toolchain_exelinkflags" FORCE)
SET(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "-shared")
SET(CMAKE_SHARED_LIBRARY_PREFIX "lib")
SET(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
SET(CMAKE_SHARED_LIBRARY_SONAME_C_FLAG "")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
SET(CMAKE_SKIP_BUILD_RPATH  TRUE)
SET(CMAKE_TARGET_WORD_BITS 32)

set(PORT JSCOnly)
set(WTF_CPU_$ENV{WTF_CPU} 1)
set(JavaScriptCore_LIBRARY_TYPE STATIC)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Os -g -DNDEBUG -flto=4")
