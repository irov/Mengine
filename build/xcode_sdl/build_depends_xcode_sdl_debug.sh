#! /bin/bash

CONFIGURATION=Debug

mkdir -p ../../solutions/dependencies_xcode_sdl/$CONFIGURATION
pushd ../../solutions/dependencies_xcode_sdl/$CONFIGURATION
/Applications/CMake.app/Contents/bin/cmake -G"Xcode" "$PWD/../../../cmake/Depends_Xcode_SDL" -DCMAKE_BUILD_TYPE:STRING=$CONFIGURATION -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk -DCMAKE_OSX_DEPLOYMENT_TARGET=10.9
/Applications/CMake.app/Contents/bin/cmake --build ./ --config $CONFIGURATION
popd
