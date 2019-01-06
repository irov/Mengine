#! /bin/bash

mkdir -p ../../build_temp/downloads
pushd ../../build_temp/downloads
/Applications/CMake.app/Contents/bin/cmake "$PWD/../../CMake/Downloads"
popd

pushd ../../build_temp/downloads
/Applications/CMake.app/Contents/bin/cmake --build ./
popd
