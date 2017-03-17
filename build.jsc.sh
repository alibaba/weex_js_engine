export NDK_ROOT=`dirname $(which ndk-build)`
export APILEVEL=21
export BINARY_PATH=`dirname $(find $NDK_ROOT/toolchains $NDK_ROOT/build  -name 'arm*' -name '*-g++' | sort  -r| head -n 1)`/../arm-linux-androideabi/bin
export TOOLCHAIN_PATH=$NDK_ROOT/toolchains/llvm/prebuilt/linux-x86_64/bin
mkdir -p build
cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release .. && ninja
