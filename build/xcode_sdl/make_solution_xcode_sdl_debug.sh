#! /bin/bash

mkdir -p ../../solutions/solution_xcode_sdl_debug
pushd ../../solutions/solution_xcode_sdl_debug
/Applications/CMake.app/Contents/bin/cmake -G"Xcode" "$PWD/../../CMake/Xcode_SDL" -DCMAKE_BUILD_TYPE:STRING='Debug' -DCMAKE_CONFIGURATION_TYPES:STRING='Debug' -DMENGINE_LIB_DIR:STRING='build_xcode_sdl_debug' -DCMAKE_OSX_SYSROOT=/Developer/SDKs/MacOSX10.9.sdk/ -DCMAKE_OSX_DEPLOYMENT_TARGET=10.9
popd