#!/bin/bash

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
SOLUTION_DIR=$PROJECT_DIR/solutions/solution_xcode_macos/$CONFIGURATION

sh "$SCRIPT_DIR/make_solution_xcode_macos.sh" "$CONFIGURATION" "$DEPLOY_PATH" "$BUILD_NUMBER" "$BUILD_VERSION"

if [ $? -ne 0 ]; then
    exit 1
fi

PROJECT="$SOLUTION_DIR/Arena3D_Xcode_MacOS.xcodeproj"

if test ! -d "$PROJECT"; then
    echo "Arena3D Xcode project was not generated: $PROJECT"
    exit 1
fi

xcodebuild -project "$PROJECT" \
    -target Arena3DServer \
    -target Arena3DBotClient \
    -target Arena3DNetProxy \
    -configuration "$CONFIGURATION" \
    CODE_SIGNING_ALLOWED=NO \
    build

if test -n "$CODE_SIGNING_ALLOWED"; then
    xcodebuild -project "$PROJECT" \
        -target MacOSApplication \
        -configuration "$CONFIGURATION" \
        CODE_SIGNING_ALLOWED="$CODE_SIGNING_ALLOWED" \
        build
else
    xcodebuild -project "$PROJECT" \
        -target MacOSApplication \
        -configuration "$CONFIGURATION" \
        build
fi
