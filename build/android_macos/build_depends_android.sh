#!/bin/bash

CONFIGURATION=$1

if test -z "$CONFIGURATION"; then
    echo "Invalid arguments, please select configuration."
    exit 1
fi

ANDROID_NDK_VERSION=29.0.14206865
ANDROID_CMAKE_VERSION=3.31.6

ANDROID_PLATFORM_VERSION=23
ANDROID_PLATFORM=android-$ANDROID_PLATFORM_VERSION
ANDROID_SDK=$HOME/Library/Android/sdk
ANDROID_NDK=$ANDROID_SDK/ndk/$ANDROID_NDK_VERSION
CMAKE_PATH=$ANDROID_SDK/cmake/$ANDROID_CMAKE_VERSION
CMAKE=$CMAKE_PATH/bin/cmake
CMAKE_GENERATOR=Ninja
CMAKE_MAKE_PROGRAM=$CMAKE_PATH/bin/ninja
CMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake

if test ! -x "$CMAKE"; then
    echo "Android SDK CMake $ANDROID_CMAKE_VERSION not found: $CMAKE"
    echo "Install CMake $ANDROID_CMAKE_VERSION from Android Studio SDK Manager."
    exit 1
fi

if test ! -x "$CMAKE_MAKE_PROGRAM"; then
    echo "Android SDK Ninja not found: $CMAKE_MAKE_PROGRAM"
    echo "Install CMake $ANDROID_CMAKE_VERSION from Android Studio SDK Manager."
    exit 1
fi

if test ! -f "$CMAKE_TOOLCHAIN_FILE"; then
    echo "Android NDK $ANDROID_NDK_VERSION not found: $ANDROID_NDK"
    echo "Install NDK $ANDROID_NDK_VERSION from Android Studio SDK Manager."
    exit 1
fi

SCRIPT_DIRECTORY="$(cd "$(dirname "$0")" && pwd)"
MENGINE_DIRECTORY="$(cd "$SCRIPT_DIRECTORY/../.." && pwd)"
CMAKELIST_PATH=$MENGINE_DIRECTORY/cmake/Depends_Android
SOLUTION_DIR=$MENGINE_DIRECTORY/solutions/dependencies_android_macos

for ABI in "x86" "x86_64" "armeabi-v7a" "arm64-v8a"
do
    mkdir -p $SOLUTION_DIR/$CONFIGURATION/$ABI
    pushd $SOLUTION_DIR/$CONFIGURATION/$ABI

    $CMAKE -G "$CMAKE_GENERATOR" \
            -DANDROID_PLATFORM=$ANDROID_PLATFORM \
            -DANDROID_ARM_NEON=TRUE \
            -DANDROID_ABI=$ABI \
            -DANDROID_NDK=$ANDROID_NDK \
            -DANDROID_STL=c++_shared \
            -DANDROID_TOOLCHAIN=clang \
            -DCMAKE_SYSTEM_NAME=Android \
            -DCMAKE_SYSTEM_VERSION=$ANDROID_PLATFORM_VERSION \
            -DCMAKE_BUILD_TYPE=$CONFIGURATION \
            -DCMAKE_CONFIGURATION_TYPES:STRING=$CONFIGURATION \
            -DCMAKE_MAKE_PROGRAM=$CMAKE_MAKE_PROGRAM \
            -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE \
            -S "$CMAKELIST_PATH" || exit 1

    $CMAKE --build ./ --config $CONFIGURATION -- -j8 || exit 1

    popd

done
