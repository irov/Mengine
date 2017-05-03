#! /bin/bash

mkdir -p ../../solutions/solution_xcode_sdl_release
pushd ../../solutions/solution_xcode_sdl_release
/Applications/CMake.app/Contents/bin/cmake -G"Xcode" "$PWD/../../CMake/Xcode_SDL" -DCMAKE_BUILD_TYPE:STRING='Release' -DCMAKE_CONFIGURATION_TYPES:STRING='Release' -DMENGINE_LIB_DIR:STRING='build_xcode_sdl_release'
popd
