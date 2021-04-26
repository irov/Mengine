@echo off

set "NAME=TicTacToe"
set "VERSION=15"
set "YEAR=2017"
set "CONFIGURATION=Debug"
set "SOLUTION_DIR=..\solutions\msvc%VERSION%_dx11\%CONFIGURATION%"

@pushd ..
@mkdir %SOLUTION_DIR%
@pushd %SOLUTION_DIR%
CMake -G "Visual Studio %VERSION% %YEAR%" -S "%CD%\..\..\..\cmake\Win32_DX11" -DCMAKE_CONFIGURATION_TYPES:STRING=%CONFIGURATION% -DCMAKE_BUILD_TYPE:STRING=%CONFIGURATION%
@popd
@popd

@echo on
@pause
