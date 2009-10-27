@echo off

@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\src" "..\build_mingw" "MinGW Makefiles" Release "-DMENGINE_LIB_DIR:STRING='build_mingw'"
@pushd ..\build_mingw\Release
mingw32-make.exe
@popd

::"..\dependencies\cmake\bin\cmake.exe" -DCMAKE_CONFIGURATION_TYPES:STRING="Debug;Release" -G"Visual Studio 8 2005" -DMENGINE_LIB_DIR:STRING="build_msvc8" ../src
@if NOT "%1"=="/NOPAUSE" pause
@echo on
