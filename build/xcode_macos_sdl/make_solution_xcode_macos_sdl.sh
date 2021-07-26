#! /bin/bash

if test -f "/usr/local/bin/cmake"; then
    CMAKE=/usr/local/bin/cmake
fi

if test -f "/Applications/CMake.app/Contents/bin/cmake"; then
    CMAKE=/Applications/CMake.app/Contents/bin/cmake
fi

CONFIGURATION=$1
DEPLOY_PATH=$2

mkdir -p ../../solutions/solution_xcode_macos_sdl/$CONFIGURATION
pushd ../../solutions/solution_xcode_macos_sdl/$CONFIGURATION
$CMAKE -G"Xcode" "$PWD/../../../cmake/Xcode_MacOS_SDL" -DCMAKE_BUILD_TYPE:STRING=$CONFIGURATION -DCMAKE_CONFIGURATION_TYPES:STRING="$CONFIGURATION" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.9 -DMENGINE_XCODE_DEPLOY_PATH="$DEPLOY_PATH"
popd
