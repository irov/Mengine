#! /bin/bash

if test -f "/usr/local/bin/cmake"; then
    CMAKE=/usr/local/bin/cmake
fi

if test -f "/Applications/CMake.app/Contents/bin/cmake"; then
    CMAKE=/Applications/CMake.app/Contents/bin/cmake
fi

CONFIGURATION=$1

if test -z "$CONFIGURATION"; then
    echo "please setup CONFIGURATION"
    exit 0
fi


mkdir -p ../../solutions/dependencies_xcode_ios_simulator_sdl/$CONFIGURATION
pushd ../../solutions/dependencies_xcode_ios_simulator_sdl/$CONFIGURATION
$CMAKE -G"Xcode" "$PWD/../../../cmake/Depends_Xcode_IOS_Simulator_SDL" -DCMAKE_BUILD_TYPE:STRING=$CONFIGURATION
$CMAKE --build ./ --config $CONFIGURATION
popd
