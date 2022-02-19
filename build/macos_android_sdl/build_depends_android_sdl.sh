#! /bin/bash

CONFIGURATION=$1

ANDROID_NDK_VERSION=22.0.7026061
ANDROID_CMAKE_VERSION=3.10.2.4988404
ANDROID_PLATFORM=android-19
ANDROID_SDK=~/Library/Android/sdk
ANDROID_NDK=~/Library/Android/sdk/ndk/$ANDROID_NDK_VERSION
CMAKE_GENERATOR="Ninja"
CMAKE_PATH=$ANDROID_SDK/cmake/$ANDROID_CMAKE_VERSION
CMAKE=$CMAKE_PATH/bin/cmake
CMAKE_MAKE_PROGRAM=$CMAKE_PATH/bin/ninja
CMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake
CMAKELIST_PATH=$PWD/../../cmake/Depends_Android_SDL
BUILD_TEMP_DIR=$PWD/../../solutions/dependencies_android_sdl

echo $PWD

for ABI in "x86" "x86_64" "armeabi-v7a" "arm64-v8a"
do
    mkdir -p $PWD/../../solutions/dependencies_android_sdl/$CONFIGURATION/$ABI
    pushd $PWD/../../solutions/dependencies_android_sdl/$CONFIGURATION/$ABI

    $CMAKE -G $CMAKE_GENERATOR \
            -DANDROID_PLATFORM=$ANDROID_PLATFORM \
            -DANDROID_ARM_NEON=TRUE \
            -DANDROID_ABI=$ABI \
            -DANDROID_NDK=$ANDROID_NDK \
            -DANDROID_STL=c++_shared \
            -DANDROID_TOOLCHAIN=clang \
            -DCMAKE_BUILD_TYPE=$CONFIGURATION \
            -DCMAKE_CONFIGURATION_TYPES:STRING=$CONFIGURATION \
            -DCMAKE_MAKE_PROGRAM=$CMAKE_MAKE_PROGRAM \
            -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE \
            -S $CMAKELIST_PATH
    
    $CMAKE --build ./ --config $CONFIGURATION -- -j8
    popd

done