#! /bin/bash

sh ./cmake_configure.sh $PWD/../dependencies/CMake/CMake.app/Contents/bin/cmake $PWD/../dependencies/cmake_scripts ../dependencies/build_macosx "Unix Makefiles" Release

last_dir=$PWD
cd ../dependencies/build_macosx/Release
make
cd $last_dir

