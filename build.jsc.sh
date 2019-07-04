#!/bin/bash
usage() {
cat<<EOFHELP
    Options:
      -t {arm|arm64|x86} the v8 engine abi type, default is arm
        'arm'=armeabi-v7a,
        'arm64'=arm64-v8a,
        'x86'=x86
      -c {clang|gcc} the compiler engine type, default is clang
        'clang'=clang,
        'gcc'=gcc7.2
EOFHELP
}

TARGET=arm
BUILD_ENGINE=gcc
while getopts "t:c:h" arg
do
    case $arg in
        t)
            [[ "$OPTARG" == 'arm' ]] && { TARGET='arm'; }
            [[ "$OPTARG" == 'arm64' ]] && { TARGET='arm64'; }
            [[ "$OPTARG" == 'x86' ]] && { TARGET='x86'; }
            ;;
        c)
            [[ "$OPTARG" == 'clang' ]] && { BUILD_ENGINE='clang'; }
            [[ "$OPTARG" == 'gcc' ]] && { BUILD_ENGINE='gcc'; }
            ;;
        h)
            usage; exit 0;
            ;;
        ?)
            echo 'unkown options'
            ;;
    esac
done

# if [ $# -eq 0 ]; then
#     TARGET=arm
# else
#     TARGET=$1
# fi
echo $TARGET
echo $BUILD_ENGINE

export BUILD_ENGINE=$BUILD_ENGINE
export NDK_ROOT=`dirname $(which ndk-build)`
echo $NDK_ROOT
export APILEVEL=16

if [ $TARGET == 'arm64' ]; then
    APILEVEL=21
fi

echo $APILEVEL

HOST=$(uname -s)-$(uname -m)
export HOST=$(echo $HOST | tr '[:upper:]' '[:lower:]')
#export TOOLCHAIN_PATH=$NDK_ROOT/toolchains/llvm/prebuilt/$HOST/bin
#export TOOLCHAIN_PATH=$HOME/my-android-toolchain

# create standalone toolchain, this dependent on android NDK
TOOLCHAIN_INSTALL_FILE=$NDK_ROOT/build/tools/make_standalone_toolchain.py
#PATH_TARGET=
#if [ $TARGET = "arm64" ]; then
#    PATH_TARGET=arm
#else
#    PATH_TARGET=TARGET
#fi

TOOLCHAIN_INSTALL_PATH=$HOME/standalone_toolchain_$TARGET

# stl value in gnustl | libc++ | stlport
STL=libc++

"$TOOLCHAIN_INSTALL_FILE" --arch $TARGET --stl=$STL --api=$APILEVEL --install-dir=$TOOLCHAIN_INSTALL_PATH --force

export TOOLCHAIN_PATH=$TOOLCHAIN_INSTALL_PATH
export SYS_LIBRARY_PATH=$TOOLCHAIN_PATH/lib

if [ $STL = "libc++" ]; then
    export LIBCXX_LINK_TYPE="-static-libstdc++"
else
    export LIBCXX_LINK_TYPE="-lstdc++"
fi

if [ $TARGET = "arm" ]; then
    export TARGET
    export TARGET_ABI=armeabi-v7a
    export CLANG_CFLAGS='-target armv7-linux-android -mthumb -funwind-tables'
    export TARGET_WORD_BITS=32
    export GCC_PATH=$TOOLCHAIN_PATH/bin
    export GCC_TOOL_PRENAME=arm-linux-androideabi
    export BINARY_PATH=$TOOLCHAIN_PATH/arm-linux-androideabi/bin
    export DEFAULT_LIBRARY="-lc++_shared -lc++abi -lunwind -landroid_support"
    export WTF_CPU=ARM
    export GCC_COMPILER_FLAGS="-march=armv7-a -mthumb -mfpu=neon -mfloat-abi=softfp"
    echo "building arm $BINARY_PATH"
elif [ $TARGET = "arm64" ]; then
    export TARGET
    export TARGET_ABI=arm64-v8a
    export CLANG_CFLAGS='-target aarch64-linux-android'
    export TARGET_WORD_BITS=64
    export GCC_PATH=$TOOLCHAIN_PATH/bin
    export GCC_TOOL_PRENAME=aarch64-linux-android
    export BINARY_PATH=$GCC_PATH/aarch64-linux-android/bin
    export DEFAULT_LIBRARY="-lc++_shared -lc++abi"
    export WTF_CPU=ARM64
    export GCC_COMPILER_FLAGS=""
    echo 'building arm64'
elif [ $TARGET = "x86" ]; then
    export TARGET
    export TARGET_ABI=x86
    export CLANG_CFLAGS='-target i686-linux-android -fPIC'
    export TARGET_WORD_BITS=32
    export GCC_PATH=$TOOLCHAIN_PATH/bin
    export GCC_TOOL_PRENAME=i686-linux-android
    #`dirname $(find $NDK_ROOT/toolchains $NDK_ROOT/build  -name 'i686*' -name '*-g++' | sort  -r| head -n 1)`/../
    export BINARY_PATH=$TOOLCHAIN_PATH/i686-linux-android/bin
    export DEFAULT_LIBRARY="-lc++_shared -lc++abi -landroid_support"
    export WTF_CPU=X86_64
    export GCC_COMPILER_FLAGS="-mtune=generic"
    echo 'building x86'
else
    echo "unknown target" 1>&2
    exit 1
fi

mkdir -p build${TARGET_WORD_BITS}
cd build${TARGET_WORD_BITS}
cmake -G Ninja -DCMAKE_BUILD_TYPE=MinSizeRel .. && cmake --build .

cd ../
result=$("$GCC_PATH/$GCC_TOOL_PRENAME-readelf" -a libWTF.so | grep -i "build id")
substr=${result#*: }
echo $substr
dir=$HOME/so_backup/$substr/$TARGET_ABI
if [ -d "$dir" ]; then
    cp lib* $dir
else
    mkdir -p "$dir"
    cp lib* $dir
fi
strip=$GCC_PATH/$GCC_TOOL_PRENAME-strip
$strip -s lib*
