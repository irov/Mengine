#! /bin/bash

sh ./cmake_configure.sh $PWD/../dependencies/CMake/CMake.app/Contents/bin/cmake $PWD/../src ../build_macosx "Unix Makefiles" Release -DMENGINE_LIB_DIR:STRING=build_macosx

rm ../bin/MacOSApplication
last_dir=$PWD
cd ../build_macosx/Release
make
cd $last_dir

