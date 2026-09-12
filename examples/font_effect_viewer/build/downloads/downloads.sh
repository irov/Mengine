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

PROJECT_DIR=$PWD/../..
SOLUTION_DIR=$PROJECT_DIR/solutions/downloads

mkdir -p $SOLUTION_DIR

pushd $SOLUTION_DIR
$CMAKE -S "$PROJECT_DIR/cmake/Downloads"
$CMAKE --build ./
popd
