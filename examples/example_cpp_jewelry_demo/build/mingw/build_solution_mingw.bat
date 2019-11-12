@echo off

set "NAME=Jewelry"
set "VERSION=16"
set "CONFIGURATION=%1"
set "SOLUTION_DIR=..\solutions\mingw\%CONFIGURATION%"

@pushd ..
@mkdir %SOLUTION_DIR%
@pushd %SOLUTION_DIR%
CMake -G "MinGW Makefiles" -S "%CD%\..\..\..\cmake\MinGW" -DCMAKE_CONFIGURATION_TYPES:STRING=%CONFIGURATION% -DCMAKE_BUILD_TYPE:STRING=%CONFIGURATION%
CMake --build .\ --config %CONFIGURATION%
@popd
@popd

@echo on
@pause