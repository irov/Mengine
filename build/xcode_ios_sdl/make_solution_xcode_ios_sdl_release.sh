#! /bin/bash

mkdir -p ../../solutions/solution_xcode_ios_sdl_release
pushd ../../solutions/solution_xcode_ios_sdl_release
/Applications/CMake.app/Contents/bin/cmake -G"Xcode" "$PWD/../../CMake/Xcode_IOS_SDL" -DCMAKE_BUILD_TYPE:STRING='Release' -DCMAKE_CONFIGURATION_TYPES:STRING='Release' -DMENGINE_LIB_DIR:STRING='build_xcode_ios_sdl_release' -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk -DCMAKE_OSX_ARCHITECTURES="arm64"
popd
