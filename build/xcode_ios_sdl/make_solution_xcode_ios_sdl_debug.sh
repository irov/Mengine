#! /bin/bash

CONFIGURATION=Debug

mkdir -p ../../solutions/solution_xcode_ios_sdl/$CONFIGURATION
pushd ../../solutions/solution_xcode_ios_sdl/$CONFIGURATION
/Applications/CMake.app/Contents/bin/cmake -G"Xcode" "$PWD/../../../CMake/Xcode_IOS_SDL" -DCMAKE_BUILD_TYPE:STRING=$CONFIGURATION -DCMAKE_CONFIGURATION_TYPES:STRING=$CONFIGURATION -DMENGINE_LIB_DIR:STRING="build_xcode_ios_sdl/\"$CONFIGURATION\"" -DCMAKE_TOOLCHAIN_FILE="$PWD/../../../dependencies/ios-cmake/ios.toolchain.cmake" -DIOS_PLATFORM=OS64 -DENABLE_BITCODE=0 -DIOS_DEPLOYMENT_TARGET="9.0"
popd
