#! /bin/bash

mkdir -p ../../solutions/solution_xcode_ios_sdl_simulator_debug
pushd ../../solutions/solution_xcode_ios_sdl_simulator_debug
/Applications/CMake.app/Contents/bin/cmake -G"Xcode" "$PWD/../../../CMake/Xcode_IOS_SDL_Simulator" -DCMAKE_BUILD_TYPE:STRING='Debug' -DCMAKE_CONFIGURATION_TYPES:STRING='Debug' -DMENGINE_LIB_DIR:STRING='build_xcode_ios_sdl_simulator_debug'
popd
