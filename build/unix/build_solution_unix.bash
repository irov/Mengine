#!/bin/bash

CONFIGURATION=$1

mkdir -p ../../solutions/solution_unix/$CONFIGURATION
pushd ../../solutions/solution_unix/$CONFIGURATION
cmake -G "Unix Makefiles" -S "../../../cmake/Unix" -DCMAKE_BUILD_TYPE:STRING=$CONFIGURATION -DCMAKE_CONFIGURATION_TYPES:STRING=$CONFIGURATION
cmake --build ./ -j 8 --config $CONFIGURATION
popd