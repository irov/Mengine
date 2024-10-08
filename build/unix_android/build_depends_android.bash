#!/bin/bash

BUILD_TYPE=$1

echo Starting dependencies build $BUILD_TYPE configuration...

ANDROID_NDK_VERSION=26.3.11579264
ANDROID_CMAKE_VERSION=3.22.1

ANDROID_PLATFORM=android-21
ANDROID_SDK=/opt/android/sdk
ANDROID_NDK=$ANDROID_SDK/ndk/$ANDROID_NDK_VERSION
CMAKE_GENERATOR="Ninja"
CMAKE_PATH=$ANDROID_SDK/cmake/$ANDROID_CMAKE_VERSION
CMAKE_EXE=$CMAKE_PATH/bin/cmake
CMAKE_MAKE_PROGRAM=$CMAKE_PATH/bin/ninja
CMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake

CMAKELIST_PATH=$PWD/../../cmake/Depends_Android
SOLUTION_DIR=$PWD/../../solutions/dependencies_unix_android

function build_dependencies {
    NEON=$1
    ABI=$2

    mkdir -p $SOLUTION_DIR/$BUILD_TYPE/$ABI
    pushd $SOLUTION_DIR/$BUILD_TYPE/$ABI

    $CMAKE_EXE -G $CMAKE_GENERATOR \
        -DANDROID_PLATFORM=$ANDROID_PLATFORM \
        -DANDROID_ARM_NEON=$NEON \
        -DANDROID_ABI=$ABI \
        -DANDROID_NDK=$ANDROID_NDK \
        -DANDROID_STL=c++_shared \
        -DANDROID_TOOLCHAIN=clang \
        -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
        -DCMAKE_CONFIGURATION_TYPES:STRING=$BUILD_TYPE \
        -DCMAKE_MAKE_PROGRAM=$CMAKE_MAKE_PROGRAM \
        -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE \
        -S $CMAKELIST_PATH || exit 1

    $CMAKE_EXE --build . --config $BUILD_TYPE -- -j8 || exit 1
    
    popd
}

build_dependencies TRUE "x86"
build_dependencies TRUE "x86_64"
build_dependencies TRUE "armeabi-v7a"
build_dependencies TRUE "arm64-v8a"
