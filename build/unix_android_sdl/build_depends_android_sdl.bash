#!/bin/bash

BUILD_TYPE=$1

echo Starting dependencies build $BUILD_TYPE configuration...

ANDROID_PLATFORM=android-19
CMAKE_GENERATOR="Ninja"
CMAKELIST_PATH=$PWD/../../cmake/Depends_Android_SDL
SOLUTION_DIR=$PWD/../../solutions/dependencies_android_sdl

function build_dependencies {
    mkdir -p $SOLUTION_DIR/$BUILD_TYPE/$1
    pushd $SOLUTION_DIR/$BUILD_TYPE/$1

    cmake -G $CMAKE_GENERATOR \
        -DANDROID_PLATFORM=$ANDROID_PLATFORM \
        -DANDROID_ARM_NEON=TRUE \
        -DANDROID_ABI=$1 \
        -DANDROID_STL=c++_shared \
        -DANDROID_TOOLCHAIN=clang \
        -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
        -DCMAKE_CONFIGURATION_TYPES:STRING=$BUILD_TYPE \
        -S $CMAKELIST_PATH

    cmake --build . --config $BUILD_TYPE
    
    status=$?

    popd

    return $status
}

if ! build_dependencies x86; then exit 1
if ! build_dependencies x86_64; then exit 1
if ! build_dependencies armeabi-v7a; then exit 1
if ! build_dependencies arm64-v8a; then exit 1