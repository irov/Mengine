#! /bin/bash

if test -f "/usr/local/bin/cmake"; then
    CMAKE=/usr/local/bin/cmake
fi

if test -f "/Applications/CMake.app/Contents/bin/cmake"; then
    CMAKE=/Applications/CMake.app/Contents/bin/cmake
fi

mkdir -p ../../solutions/downloads
pushd ../../solutions/downloads
$CMAKE $PWD/../../cmake/Downloads
popd

pushd ../../solutions/downloads
$CMAKE --build ./
popd
