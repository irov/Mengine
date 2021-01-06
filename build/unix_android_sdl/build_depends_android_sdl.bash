#!/bin/bash

BUILD_TYPE=$1

echo Starting dependencies build $BUILD_TYPE configuration...

ANDROID_PLATFORM=android-19
CMAKE_GENERATOR="Ninja"
CMAKELIST_PATH=$PWD/../../cmake/Depends_Android_SDL
SOLUTION_DIR=$PWD/../../solutions/dependencies_android_sdl

mkdir -p $SOLUTION_DIR/$BUILD_TYPE/armeabi-v7a
pushd $SOLUTION_DIR/$BUILD_TYPE/armeabi-v7a

cmake -G $CMAKE_GENERATOR \
    -DANDROID_PLATFORM=$ANDROID_PLATFORM \
    -DANDROID_ARM_NEON=TRUE \
    -DANDROID_ABI=armeabi-v7a \
    -DANDROID_STL=c++_shared \
    -DANDROID_TOOLCHAIN=clang \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_CONFIGURATION_TYPES:STRING=$BUILD_TYPE \
    -S $CMAKELIST_PATH

cmake --build . --config $BUILD_TYPE

popd

mkdir -p $SOLUTION_DIR/$BUILD_TYPE/arm64-v8a
pushd $SOLUTION_DIR/$BUILD_TYPE/arm64-v8a

cmake -G $CMAKE_GENERATOR \
    -DANDROID_PLATFORM=$ANDROID_PLATFORM \
    -DANDROID_ARM_NEON=TRUE \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_STL=c++_shared \
    -DANDROID_TOOLCHAIN=clang \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_CONFIGURATION_TYPES:STRING=$BUILD_TYPE \
    -S $CMAKELIST_PATH

cmake --build . --config $BUILD_TYPE

popd

[ $? -eq 0 ]  || exit 1
