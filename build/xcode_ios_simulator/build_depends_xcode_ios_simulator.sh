#! /bin/bash

CMAKE=cmake

if test -f "/usr/local/bin/cmake"; then
    CMAKE=/usr/local/bin/cmake
fi

if test -f "/Applications/CMake.app/Contents/bin/cmake"; then
    CMAKE=/Applications/CMake.app/Contents/bin/cmake
fi

CONFIGURATION=$1

if test -z "$CONFIGURATION"; then
    echo "please setup CONFIGURATION"
    exit 1
fi

SOLUTION_NAME=dependencies_xcode_ios_simulator
SOLUTION_DIR=$PWD/../../solutions/$SOLUTION_NAME/$CONFIGURATION
SOURCE_DIRECTORY=$PWD/../../cmake/Depends_Xcode_iOS_Simulator

mkdir -p "$SOLUTION_DIR"

pushd "$SOLUTION_DIR"
"$CMAKE" -G"Xcode" -S "$SOURCE_DIRECTORY" -DCMAKE_BUILD_TYPE:STRING="$CONFIGURATION" -DCMAKE_CONFIGURATION_TYPES:STRING="$CONFIGURATION" -DCMAKE_DISABLE_PRECOMPILE_HEADERS=ON

if [ $? -ne 0 ]; then
    echo "please fix generate CMake"
    exit 1
fi
popd

SIMULATOR_ARCH=$(uname -m)

pushd "$SOLUTION_DIR"
"$CMAKE" --build ./ --config "$CONFIGURATION" -- -sdk iphonesimulator ARCHS="$SIMULATOR_ARCH" ONLY_ACTIVE_ARCH=YES

if [ $? -ne 0 ]; then
    echo "please fix build CMake"
    exit 1
fi
popd

exit 0
