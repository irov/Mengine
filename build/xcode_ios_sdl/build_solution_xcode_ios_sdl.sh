#! /bin/bash

if test -f "/usr/local/bin/cmake"; then
    CMAKE=/usr/local/bin/cmake
fi

if test -f "/Applications/CMake.app/Contents/bin/cmake"; then
    CMAKE=/Applications/CMake.app/Contents/bin/cmake
fi

CONFIGURATION=$1
DEPLOY_PATH=$2
BUILD_NUMBER=$3
BUILD_VERSION=$4

if test -z "$CONFIGURATION"; then
    echo "please setup CONFIGURATION"
    exit 1
fi

if test -z "$DEPLOY_PATH"; then
    echo "please setup DEPLOY_PATH"
    exit 1
fi

if test -z "$BUILD_NUMBER"; then
    echo "please setup BUILD_NUMBER"
    exit 1
fi

if test -z "$BUILD_VERSION"; then
    echo "please setup BUILD_VERSION"
    exit 1
fi

SOLUTION_NAME=solution_xcode_ios_sdl
SOLUTION_DIR=$PWD/../../solutions/$SOLUTION_NAME/$CONFIGURATION
SOURCE_DIRECTORY=$PWD/../../cmake/Xcode_IOS_SDL
DEPENDENCIES_PROJECT=Depends_Xcode_IOS_SDL

mkdir -p $SOLUTION_DIR

pushd $SOLUTION_DIR
$CMAKE -G"Xcode" -S "$SOURCE_DIRECTORY" -DCMAKE_BUILD_TYPE:STRING=$CONFIGURATION -DCMAKE_CONFIGURATION_TYPES:STRING="$CONFIGURATION" -DMENGINE_DEPENDENCIES_PROJECT="$DEPENDENCIES_PROJECT" -DMENGINE_DEPLOY_PATH:STRING="$DEPLOY_PATH" -DMENGINE_BUILD_NUMBER:STRING="$BUILD_NUMBER" -DMENGINE_BUILD_VERSION:STRING="$BUILD_VERSION"

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

pushd $SOLUTION_DIR
$CMAKE --build ./ --config $CONFIGURATION

if [ $? -ne 0 ]; then
    echo "please fix build CMake"
    exit 1
fi
popd

exit 0