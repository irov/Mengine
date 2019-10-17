#! /bin/bash

CONFIGURATION=Release

mkdir -p ../../solutions/solution_xcode_sdl/$CONFIGURATION
pushd ../../solutions/solution_xcode_sdl/$CONFIGURATION
/Applications/CMake.app/Contents/bin/cmake -G"Xcode" "$PWD/../../../cmake/Xcode_SDL" -DCMAKE_BUILD_TYPE:STRING='$CONFIGURATION' -DCMAKE_CONFIGURATION_TYPES:STRING='$CONFIGURATION' -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk -DCMAKE_OSX_DEPLOYMENT_TARGET=10.9
popd
