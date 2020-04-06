#! /bin/bash

CONFIGURATION=Release

mkdir -p ../../solutions/dependencies_xcode_ios_sdl/$CONFIGURATION
pushd ../../solutions/dependencies_xcode_ios_sdl/$CONFIGURATION
/Applications/CMake.app/Contents/bin/cmake -G"Xcode" "$PWD/../../../cmake/Depends_Xcode_IOS_SDL" -DCMAKE_BUILD_TYPE:STRING=$CONFIGURATION 
/Applications/CMake.app/Contents/bin/cmake --build ./ --config $CONFIGURATION
popd
