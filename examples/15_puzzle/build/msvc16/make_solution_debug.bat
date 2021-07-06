@echo off

set "NAME=TicTacToe"
set "VERSION=16"
set "YEAR=2019"
set "CONFIGURATION=Debug"
set "SOLUTION_DIR=..\solutions\msvc%VERSION%\%CONFIGURATION%"

@pushd ..
@mkdir %SOLUTION_DIR%
@pushd %SOLUTION_DIR%
CMake -G "Visual Studio %VERSION% %YEAR%" -A Win32 -S "%CD%\..\..\..\cmake\Win32" -DCMAKE_CONFIGURATION_TYPES:STRING=%CONFIGURATION% -DCMAKE_BUILD_TYPE:STRING=%CONFIGURATION%
@popd
@popd

@echo on
@pause
