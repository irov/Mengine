#! /bin/bash

cmake_tool=$1
source_dir=$2
build_dir=$3
generator=$4
configuration=$5
cmake_params=$6
cmake_params2=$7

mkdir -p $build_dir/$configuration
last_dir=$PWD
cd $build_dir/$configuration
echo $cmake_tool -G"$generator" $source_dir -DCMAKE_BUILD_TYPE:STRING=$configuration $cmake_params $cmake_params2
$cmake_tool -G"$generator" $source_dir -DCMAKE_BUILD_TYPE:STRING=$configuration $cmake_params $cmake_params2
cd $last_dir