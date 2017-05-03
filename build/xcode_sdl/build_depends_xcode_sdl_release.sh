#! /bin/bash

mkdir -p ../../build_temp/build_xcode_sdl_release
pushd ../../build_temp/build_xcode_sdl_release
/Applications/CMake.app/Contents/bin/cmake -G"Xcode" "$PWD/../../CMake/Depends_Xcode_SDL" -DCMAKE_BUILD_TYPE:STRING='Release' -DCMAKE_CONFIGURATION_TYPES:STRING='Release'
/Applications/CMake.app/Contents/bin/cmake --build ./ --config Release
popd