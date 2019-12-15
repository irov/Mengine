#! /bin/bash

CONFIGURATION=Debug

mkdir -p ../../solutions/solution_xcode_sdl/$CONFIGURATION
pushd ../../solutions/solution_xcode_sdl/$CONFIGURATION
/Applications/CMake.app/Contents/bin/cmake -G"Xcode" "$PWD/../../../cmake/Xcode_SDL" -DCMAKE_BUILD_TYPE:STRING=$CONFIGURATION -DCMAKE_CONFIGURATION_TYPES:STRING=$CONFIGURATION -DCMAKE_OSX_DEPLOYMENT_TARGET=10.9
popd
