#! /bin/bash

CONFIGURATION=$1
DEPLOY_PATH=$2
BUILD_NUMBER=$3
BUILD_VERSION=$4

if test -z "$CONFIGURATION"; then
    echo "please setup CONFIGURATION"
    exit 1
fi

PROJECT_DIR=$PWD/../..
SOLUTION_DIR=$PROJECT_DIR/solutions/solution_xcode_macos/$CONFIGURATION

sh ./make_solution_xcode_macos.sh $CONFIGURATION "$DEPLOY_PATH" "$BUILD_NUMBER" "$BUILD_VERSION"

if [ $? -ne 0 ]; then
    exit 1
fi

WORKSPACE="$SOLUTION_DIR/Mengine_Xcode_MacOS.xcworkspace"

if test -d "$WORKSPACE"; then
    xcodebuild -workspace "$WORKSPACE" -scheme MacOSApplication -configuration "$CONFIGURATION" build
else
    cmake --build "$SOLUTION_DIR" --config "$CONFIGURATION"
fi
