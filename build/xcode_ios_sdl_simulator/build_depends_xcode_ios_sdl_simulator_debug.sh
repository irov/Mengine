#! /bin/bash

mkdir -p ../../build_temp/build_xcode_ios_sdl_simulator_debug
pushd ../../build_temp/build_xcode_ios_sdl_simulator_debug
/Applications/CMake.app/Contents/bin/cmake -G"Xcode" "$PWD/../../CMake/Depends_Xcode_IOS_SDL_Simulator" -DCMAKE_BUILD_TYPE:STRING='Debug' -DCMAKE_CONFIGURATION_TYPES:STRING='Debug' -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator.sdk
popd
