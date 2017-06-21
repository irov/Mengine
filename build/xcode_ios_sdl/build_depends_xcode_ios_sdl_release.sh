#! /bin/bash

mkdir -p ../../build_temp/build_xcode_ios_sdl_release
pushd ../../build_temp/build_xcode_ios_sdl_release
/Applications/CMake.app/Contents/bin/cmake -G"Xcode" "$PWD/../../CMake/Depends_Xcode_IOS_SDL" -DCMAKE_BUILD_TYPE:STRING='Release' -DCMAKE_CONFIGURATION_TYPES:STRING='Release' -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk -DCMAKE_OSX_ARCHITECTURES="armv7, arm64"
popd
