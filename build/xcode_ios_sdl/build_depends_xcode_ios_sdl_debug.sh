#! /bin/bash

CONFIGURATION=Debug

mkdir -p ../../build_temp/build_xcode_ios_sdl/$CONFIGURATION
pushd ../../build_temp/build_xcode_ios_sdl/$CONFIGURATION
/Applications/CMake.app/Contents/bin/cmake -G"Xcode" "$PWD/../../CMake/Depends_Xcode_IOS_SDL" -DCMAKE_BUILD_TYPE:STRING='$CONFIGURATION' -DCMAKE_CONFIGURATION_TYPES:STRING='$CONFIGURATION' -DCMAKE_C_COMPILER_WORKS:BOOL=TRUE -DCMAKE_CXX_COMPILER_WORKS:BOOL=TRUE -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk -DCMAKE_OSX_ARCHITECTURES=${ARCHS_STANDARD}
popd
