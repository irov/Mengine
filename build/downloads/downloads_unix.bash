#!/bin/bash

mkdir -p ../../solutions/downloads
pushd ../../solutions/downloads
cmake "$PWD/../../cmake/Downloads"
popd

pushd ../../solutions/downloads
cmake --build ./
popd
