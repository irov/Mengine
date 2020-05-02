#! /bin/bash

CONFIGURATION=Release

mkdir -p ../../solutions/solution_xcode_ios_sdl/$CONFIGURATION
pushd ../../solutions/solution_xcode_ios_sdl/$CONFIGURATION
/Applications/CMake.app/Contents/bin/cmake -G"Xcode" "$PWD/../../../cmake/Xcode_IOS_SDL" -DCMAKE_BUILD_TYPE:STRING=$CONFIGURATION -DCMAKE_CONFIGURATION_TYPES:STRING="$CONFIGURATION"
popd
