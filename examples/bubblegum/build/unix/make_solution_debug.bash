#!/bin/bash

CONFIGURATION=$1

mkdir -p ../../solutions/solution_unix/$CONFIGURATION
pushd ../../solutions/solution_unix/$CONFIGURATION
cmake -G "Unix Makefiles" -S "%CD%/../../../cmake/Unix" -DCMAKE_BUILD_TYPE:STRING=$CONFIGURATION -DCMAKE_CONFIGURATION_TYPES:STRING=$CONFIGURATION
popd