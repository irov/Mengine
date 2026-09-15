#! /bin/bash

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

PROJECT_DIR=$PWD/../..

if test -z "$DEPLOY_PATH"; then
    DEPLOY_PATH=$PROJECT_DIR/resources
fi

if test -z "$BUILD_NUMBER"; then
    BUILD_NUMBER=0
fi

if test -z "$BUILD_VERSION"; then
    BUILD_VERSION=0.0.0
fi

SOLUTION_NAME=solution_xcode_macos
SOLUTION_DIR=$PROJECT_DIR/solutions/$SOLUTION_NAME/$CONFIGURATION
SOURCE_DIRECTORY=$PROJECT_DIR/cmake/Xcode_MacOS

mkdir -p $SOLUTION_DIR

pushd $SOLUTION_DIR
$CMAKE -G"Xcode" -S "$SOURCE_DIRECTORY" -DCMAKE_BUILD_TYPE:STRING=$CONFIGURATION -DCMAKE_CONFIGURATION_TYPES:STRING="$CONFIGURATION" -DMENGINE_DEPLOY_PATH:STRING="$DEPLOY_PATH" -DMENGINE_BUILD_NUMBER:STRING="$BUILD_NUMBER" -DMENGINE_BUILD_VERSION:STRING="$BUILD_VERSION"

if [ $? -ne 0 ]; then
    echo "please fix generate CMake"
    exit 1
fi
popd

pushd $SOLUTION_DIR
if test -f "Podfile"; then
    pod install --repo-update

    if [ $? -ne 0 ]; then
        echo "please fix Cocoapods"
        exit 1
    fi
fi
popd

exit 0
