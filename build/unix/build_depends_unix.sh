#!/bin/bash

CONFIGURATION=$1

mkdir -p ../../solutions/dependencies_unix/$CONFIGURATION
pushd ../../solutions/dependencies_unix/$CONFIGURATION
cmake -G "Unix Makefiles" -S "../../../cmake/Depends_Unix" -DCMAKE_BUILD_TYPE:STRING=$CONFIGURATION
cmake --build ./ --config $CONFIGURATION
popd