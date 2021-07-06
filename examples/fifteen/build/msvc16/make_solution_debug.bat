@echo off

set "NAME=Fifteen"
set "VERSION=16"
set "CONFIGURATION=Debug"
set "SOLUTION_DIR=..\solutions\msvc%VERSION%\%CONFIGURATION%"

@pushd ..
@mkdir %SOLUTION_DIR%
@pushd %SOLUTION_DIR%
CMake -G "Visual Studio 16 2019" -A Win32 -S "%CD%\..\..\..\cmake\Win32" -DCMAKE_CONFIGURATION_TYPES:STRING=%CONFIGURATION% -DCMAKE_BUILD_TYPE:STRING=%CONFIGURATION%
@popd
@popd

@echo on
@pause
