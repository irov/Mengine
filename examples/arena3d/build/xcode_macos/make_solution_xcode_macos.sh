#!/bin/bash

if test -f "/usr/local/bin/cmake"; then
    CMAKE=/usr/local/bin/cmake
fi

if test -f "/Applications/CMake.app/Contents/bin/cmake"; then
    CMAKE=/Applications/CMake.app/Contents/bin/cmake
fi

if test -z "$CMAKE"; then
    CMAKE=cmake
fi

CONFIGURATION=$1
DEPLOY_PATH=$2
BUILD_NUMBER=$3
BUILD_VERSION=$4

if test -z "$CONFIGURATION"; then
    echo "please setup CONFIGURATION"
    exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/../.." && pwd)"

if test -z "$DEPLOY_PATH"; then
    DEPLOY_PATH=$PROJECT_DIR/resources
fi

if test -z "$BUILD_NUMBER"; then
    BUILD_NUMBER=1
fi

if test -z "$BUILD_VERSION"; then
    BUILD_VERSION=1.0.0
fi

SOLUTION_NAME=solution_xcode_macos
SOLUTION_DIR=$PROJECT_DIR/solutions/$SOLUTION_NAME/$CONFIGURATION
SOURCE_DIRECTORY=$PROJECT_DIR/cmake/Xcode_MacOS
DEPENDENCIES_PROJECT=Depends_Xcode_MacOS

mkdir -p "$SOLUTION_DIR"

pushd "$SOLUTION_DIR" || exit 1
"$CMAKE" -G "Xcode" -S "$SOURCE_DIRECTORY" \
    -DCMAKE_BUILD_TYPE:STRING="$CONFIGURATION" \
    -DCMAKE_CONFIGURATION_TYPES:STRING="$CONFIGURATION" \
    -DMENGINE_DEPENDENCIES_PROJECT:STRING="$DEPENDENCIES_PROJECT" \
    -DMENGINE_DEPLOY_PATH:STRING="$DEPLOY_PATH" \
    -DMENGINE_BUILD_NUMBER:STRING="$BUILD_NUMBER" \
    -DMENGINE_BUILD_VERSION:STRING="$BUILD_VERSION"

if [ $? -ne 0 ]; then
    echo "please fix generate CMake"
    exit 1
fi
popd || exit 1

pushd "$SOLUTION_DIR" || exit 1
if test -f "Podfile"; then
    pod install --repo-update

    if [ $? -ne 0 ]; then
        echo "please fix Cocoapods"
        exit 1
    fi
fi
popd || exit 1

echo "Arena3D Xcode solution: $SOLUTION_DIR/Arena3D_Xcode_MacOS.xcodeproj"
exit 0
