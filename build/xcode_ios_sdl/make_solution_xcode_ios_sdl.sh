#! /bin/bash

if test -f "/usr/local/bin/cmake"; then
    CMAKE=/usr/local/bin/cmake
fi

if test -f "/Applications/CMake.app/Contents/bin/cmake"; then
    CMAKE=/Applications/CMake.app/Contents/bin/cmake
fi

CONFIGURATION=$1
DEPLOY_PATH=$2

mkdir -p ../../solutions/solution_xcode_ios_sdl/$CONFIGURATION
pushd ../../solutions/solution_xcode_ios_sdl/$CONFIGURATION
$CMAKE -G"Xcode" "$PWD/../../../cmake/Xcode_IOS_SDL" -DCMAKE_BUILD_TYPE:STRING=$CONFIGURATION -DCMAKE_CONFIGURATION_TYPES:STRING="$CONFIGURATION" -DMENGINE_XCODE_DEPLOY_PATH="$DEPLOY_PATH"
popd
