#! /bin/bash

CONFIGURATION=Debug

mkdir -p ../../solutions/dependencies_xcode_ios_sdl/$CONFIGURATION
pushd ../../solutions/dependencies_xcode_ios_sdl/$CONFIGURATION
/Applications/CMake.app/Contents/bin/cmake -G"Xcode" "$PWD/../../../cmake/Depends_Xcode_IOS_SDL" -DCMAKE_BUILD_TYPE:STRING=$CONFIGURATION -DCMAKE_CONFIGURATION_TYPES:STRING=$CONFIGURATION -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/ -DCMAKE_TOOLCHAIN_FILE="$PWD/../../../dependencies/ios-cmake/ios.toolchain.cmake" -DPLATFORM=OS64 -DENABLE_BITCODE=0 -DDEPLOYMENT_TARGET="9.0"
popd
