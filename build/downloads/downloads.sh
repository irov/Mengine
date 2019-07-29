#! /bin/bash

mkdir -p ../../solutions/downloads
pushd ../../solutions/downloads
/Applications/CMake.app/Contents/bin/cmake "$PWD/../../CMake/Downloads"
popd

pushd ../../solutions/downloads
/Applications/CMake.app/Contents/bin/cmake --build ./
popd
