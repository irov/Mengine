#! /bin/bash

if test -f "/usr/local/bin/cmake"; then
    CMAKE=/usr/local/bin/cmake
fi

if test -f "/Applications/CMake.app/Contents/bin/cmake"; then
    CMAKE=/Applications/CMake.app/Contents/bin/cmake
fi

CONFIGURATION=$1

mkdir -p ../../solutions/dependencies_xcode_macos_universal_sdl/$CONFIGURATION
pushd ../../solutions/dependencies_xcode_macos_universal_sdl/$CONFIGURATION
$CMAKE -G"Xcode" "$PWD/../../../cmake/Depends_Xode_MacOS_Universal_SDL" -DCMAKE_BUILD_TYPE:STRING=$CONFIGURATION -DCMAKE_OSX_SYSROOT=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk
$CMAKE --build ./ --config $CONFIGURATION
popd
