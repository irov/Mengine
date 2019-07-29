#! /bin/bash

mkdir -p ../solutions/dependencies_xcode_sdl_debug
pushd ../solutions/dependencies_xcode_sdl_debug
/Applications/CMake.app/Contents/bin/cmake -G"Xcode" "$PWD/../../CMake/Depends_Xcode_SDL" -DCMAKE_BUILD_TYPE:STRING='Debug' -DCMAKE_CONFIGURATION_TYPES:STRING='Debug'
/Applications/CMake.app/Contents/bin/cmake --build ./ --config Debug
popd