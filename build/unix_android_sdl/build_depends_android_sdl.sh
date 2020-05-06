#! /bin/bash

BUILD_TYPE=$1

echo Starting dependencies build $BUILD_TYPE configuration...

ANDROID_PLATFORM=android-18
ANDROID_CMAKE_VERSION=3.10.2.4988404
ANDROID_SDK=~/Library/Android/sdk
ANDROID_NDK=$ANDROID_SDK/ndk-bundle
ANDROID_SYSROOT=$ANDROID_NDK/sysroot
CMAKE_PATH=$ANDROID_SDK/cmake/$ANDROID_CMAKE_VERSION/bin
MAKE_PROGRAM=$CMAKE_PATH/ninja
TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake
CMAKE_EXE=$CMAKE_PATH/cmake
CMAKE_GENERATOR="Ninja"
CMAKELIST_PATH=$PWD/../../cmake/Depends_Android_SDL
SOLUTION_DIR=$PWD/../../solutions/dependencies_android_sdl

mkdir -p $SOLUTION_DIR/$BUILD_TYPE/armeabi-v7a
pushd $SOLUTION_DIR/$BUILD_TYPE/armeabi-v7a

$CMAKE_EXE -G $CMAKE_GENERATOR \
    -DANDROID_PLATFORM=$ANDROID_PLATFORM \
    -DANDROID_ARM_NEON=TRUE \
    -DANDROID_ABI=armeabi-v7a \
    -DANDROID_NDK=$ANDROID_NDK \
    -DANDROID_STL=c++_shared \
    -DANDROID_TOOLCHAIN=clang \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_CONFIGURATION_TYPES:STRING=$BUILD_TYPE \
    -DCMAKE_MAKE_PROGRAM=$MAKE_PROGRAM \
    -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE \
    -S $CMAKELIST_PATH

$CMAKE_EXE --build . --config $BUILD_TYPE

popd

mkdir -p $SOLUTION_DIR/$BUILD_TYPE/arm64-v8a
pushd $SOLUTION_DIR/$BUILD_TYPE/arm64-v8a

$CMAKE_EXE -G $CMAKE_GENERATOR \
    -DANDROID_PLATFORM=$ANDROID_PLATFORM \
    -DANDROID_ARM_NEON=TRUE \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_NDK=$ANDROID_NDK \
    -DANDROID_STL=c++_shared \
    -DANDROID_TOOLCHAIN=clang \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_CONFIGURATION_TYPES:STRING=$BUILD_TYPE \
    -DCMAKE_MAKE_PROGRAM=$MAKE_PROGRAM \
    -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE \
    -S $CMAKELIST_PATH

$CMAKE_EXE --build . --config $BUILD_TYPE

popd

echo Done