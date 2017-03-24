export NDK_ROOT=/Users/zhengshihan/android-ndk-r13b
export APILEVEL=21
export BINARY_PATH=`dirname $(find $NDK_ROOT/toolchains $NDK_ROOT/build  -name 'arm*' -name '*-g++' | sort  -r| head -n 1)`/../arm-linux-androideabi/bin
HOST=$(uname -s)-$(uname -m)
export HOST=$(echo $HOST | tr '[:upper:]' '[:lower:]')
export TOOLCHAIN_PATH=$NDK_ROOT/toolchains/llvm/prebuilt/$HOST/bin
mkdir -p build
cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=RelWithDebInfo .. && cmake --build .