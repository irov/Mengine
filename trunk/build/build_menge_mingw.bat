@echo off

@del /q ..\bin\WinApplication.exe
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\MinGW" "..\build_mingw" "MinGW Makefiles" Release "-DMENGINE_LIB_DIR:STRING='build_mingw'"
@pushd ..\build_mingw\Release
mingw32-make.exe
@popd

@if NOT "%1"=="/NOPAUSE" pause
@echo on
