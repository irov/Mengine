#! /bin/bash

if test -f "/usr/local/bin/cmake"; then
    CMAKE=/usr/local/bin/cmake
fi

if test -f "/Applications/CMake.app/Contents/bin/cmake"; then
    CMAKE=/Applications/CMake.app/Contents/bin/cmake
fi

CONFIGURATION=$1
DEPLOY_PATH=$2

if test -z "$CONFIGURATION"; then
    echo "please setup CONFIGURATION"
    exit 0
fi

if test -z "$DEPLOY_PATH"; then
    echo "please setup DEPLOY_PATH"
    exit 0
fi

mkdir -p ../../solutions/solution_xcode_macos_universal_sdl/$CONFIGURATION
pushd ../../solutions/solution_xcode_macos_universal_sdl/$CONFIGURATION
$CMAKE -G"Xcode" "$PWD/../../../cmake/Xcode_MacOS_Universal_SDL" -DCMAKE_BUILD_TYPE:STRING="$CONFIGURATION" -DCMAKE_CONFIGURATION_TYPES:STRING="$CONFIGURATION" -DMENGINE_XCODE_DEPLOY_PATH="$DEPLOY_PATH"
if [ $? -ne 0 ]; then
    echo "please fix CMake"
    exit 0
fi
pod install --repo-update
popd
