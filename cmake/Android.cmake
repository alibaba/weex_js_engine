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
SET(CMAKE_BUILD_ENGINE $ENV{BUILD_ENGINE})
string(COMPARE EQUAL "$ENV{BUILD_ENGINE}" "gcc" gcc_compaile)
if(gcc_compaile)
SET(CMAKE_TOOLCHAIN_PATH ${CMAKE_SOURCE_DIR}/arm-linux-androideabi-gcc-7.2)
SET(CMAKE_ANDROID_SYSROOT ${CMAKE_SOURCE_DIR}/arm-sysroot)
SET(CMAKE_C_COMPILER ${CMAKE_TOOLCHAIN_PATH}/bin/arm-linux-androideabi-gcc)
SET(CMAKE_CXX_COMPILER ${CMAKE_TOOLCHAIN_PATH}/bin/arm-linux-androideabi-g++)
SET(CMAKE_FIND_ROOT_PATH ${CMAKE_TOOLCHAIN_PATH})
SET(CMAKE_AR ${CMAKE_TOOLCHAIN_PATH}/bin/arm-linux-androideabi-gcc-ar)
SET(CMAKE_STRIP ${CMAKE_TOOLCHAIN_PATH}/arm-linux-androideabi/bin/strip)
SET(CMAKE_RANLIB ${CMAKE_TOOLCHAIN_PATH}/bin/arm-linux-androideabi-gcc-ranlib)
else()
SET(CMAKE_TOOLCHAIN_PATH $ENV{TOOLCHAIN_PATH})
SET(CMAKE_BINARY_PATH $ENV{BINARY_PATH})
SET(CMAKE_GCC_PATH $ENV{GCC_PATH})
SET(CMAKE_HOST_NAME $ENV{HOST})
SET(CMAKE_TARGET_NAME $ENV{TARGET})
SET(CMAKE_TARGET_ABI_NAME $ENV{TARGET_ABI})
SET(CMAKE_CLANG_CFLAGS $ENV{CLANG_CFLAGS})
SET(CMAKE_ANDROID_SYSROOT $ENV{NDK_ROOT}/platforms/android-$ENV{APILEVEL}/arch-${CMAKE_TARGET_NAME})
SET(CMAKE_ANDROID_STL_INCLUDE $ENV{NDK_ROOT}/sources/cxx-stl/llvm-libc++/include)
SET(CMAKE_ANDROID_STL_BITS_INCLUDE $ENV{NDK_ROOT}/sources/cxx-stl/llvm-libc++abi/include)
SET(CMAKE_TARGET_WORD_BITS $ENV{TARGET_WORD_BITS})
SET(CMAKE_C_COMPILER ${CMAKE_TOOLCHAIN_PATH}/clang)
SET(CMAKE_CXX_COMPILER ${CMAKE_TOOLCHAIN_PATH}/clang++)
SET(CMAKE_FIND_ROOT_PATH ${CMAKE_BINARY_PATH})
SET(CMAKE_AR ${CMAKE_BINARY_PATH}/ar)
SET(CMAKE_RANLIB ${CMAKE_BINARY_PATH}/ranlib)
SET(CMAKE_STRIP ${CMAKE_BINARY_PATH}/strip)
endif()

SET(ANDROID "1" CACHE STRING "ANDROID" FORCE)

if(gcc_compaile)
SET(ANDROID_COMMON_FLAGS "${CMAKE_CLANG_CFLAGS} --sysroot=${CMAKE_ANDROID_SYSROOT} -ffunction-sections -fdata-sections -fomit-frame-pointer -march=armv7-a -mthumb -mfpu=neon -mfloat-abi=softfp")
else()
SET(ANDROID_COMMON_FLAGS "${CMAKE_CLANG_CFLAGS} --sysroot=${CMAKE_ANDROID_SYSROOT} -isystem ${CMAKE_ANDROID_STL_INCLUDE} -isystem ${CMAKE_ANDROID_STL_BITS_INCLUDE} -B${CMAKE_BINARY_PATH} -ffunction-sections -fdata-sections -fomit-frame-pointer -gcc-toolchain ${CMAKE_GCC_PATH}")
endif()
SET(CMAKE_C_FLAGS "${ANDROID_COMMON_FLAGS}" CACHE STRING "toolchain_cflags" FORCE)
SET(CMAKE_CXX_FLAGS "${ANDROID_COMMON_FLAGS} -std=gnu++1y" CACHE STRING "toolchain_cxxflags" FORCE)
SET(CMAKE_ASM_FLAGS "${ANDROID_COMMON_FLAGS}" CACHE STRING "toolchain_asmflags" FORCE)
SET(CMAKE_EXE_LINKER_FLAGS "-pie" CACHE STRING "toolchain_exelinkflags" FORCE)
SET(CMAKE_SHARED_LINKER_FLAGS "-Wl,--gc-sections -Wl,--build-id=sha1" CACHE STRING "toolchain_exelinkflags" FORCE)
SET(CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "-shared")
SET(CMAKE_SHARED_LIBRARY_PREFIX "lib")
SET(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
SET(CMAKE_SHARED_LIBRARY_SONAME_C_FLAG "")

if(gcc_compaile)
#FIXME
else()
link_libraries($ENV{DEFAULT_LIBRARY})
link_directories($ENV{NDK_ROOT}/sources/cxx-stl/llvm-libc++/libs/${CMAKE_TARGET_ABI_NAME})
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
SET(CMAKE_SKIP_BUILD_RPATH  TRUE)
if(gcc_compaile)
SET(CMAKE_TARGET_WORD_BITS 32)
endif()

set(PORT JSCOnly)
set(WTF_CPU_$ENV{WTF_CPU} 1)
set(JavaScriptCore_LIBRARY_TYPE STATIC)

if(gcc_compaile)
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-Os -g -DNDEBUG -flto=4")
else()
endif()