
V8PATH=${0%/*}
cd $V8PATH

NDK_ROOT=`dirname $(which ndk-build)`
export ANDROID_NDK_ROOT=$NDK_ROOT

if ! which gperf || ! which bison || ! which flex || ! which perl; then
  echo "must install gperf,bison,flex,perl"
  exit 1
fi

# If not defined ANDROID_TOOLCHAIN, it search the ndk directory and find the gcc-4.4.3 to compile v8.
if [ -z "$ANDROID_TOOLCHAIN" ]; then
    if [ "$TARGET_ARCH" = "x86" ]; then
        if [ -z "$TOOLCHAIN_PATH" ]; then
		    TOOLCHAIN_PATH=`dirname $(find $NDK_ROOT/toolchains $NDK_ROOT/build  -name 'i686*' -name '*-g++' | sort -r | head -n 1)`
        fi
    elif [ "$TARGET_ARCH" = "armeabi" ]; then
        if [ -z "$TOOLCHAIN_PATH" ]; then
		    TOOLCHAIN_PATH=`dirname $(find $NDK_ROOT/toolchains $NDK_ROOT/build  -name 'arm*' -name '*-g++' | sort  -r| head -n 1)`
        fi
        EXTRA_OPTION="armv7=false vfp2=off vfp3=off"
    elif [ "$TARGET_ARCH" = "armeabi-v7a" ]; then
        if [ -z "$TOOLCHAIN_PATH" ]; then		
            TOOLCHAIN_PATH=`dirname $(find $NDK_ROOT/toolchains $NDK_ROOT/build  -name 'arm*' -name '*-g++' | sort  -r| head -n 1)`
        fi
        EXTRA_OPTION="armv7=true vfp2=on vfp3=on"
    else
        echo "unsupported platform " "$TARGET_ARCH" 1>&2
        exit 1
    fi
    export ANDROID_TOOLCHAIN=${TOOLCHAIN_PATH%/bin}
fi

if [ "$TARGET_ARCH" = "x86" ]; then
    ARCH=ia32
else
    ARCH=arm
fi

if [ "$USE_CCACHE" = "1" ] || [ "$NDK_CCACHE" = "ccache" ]; then
  export CXX_host='ccache g++'
  export CC_host='ccache gcc'
fi

make android_"$ARCH".release -j9 backtrace=off ${EXTRA_OPTION} snapshot=off
