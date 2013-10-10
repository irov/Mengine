@echo off

@echo Starting dependencies build...

@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\Depends_MinGW" "..\dependencies\build_mingw" "MinGW Makefiles" Debug
@pushd ..\dependencies\build_mingw\Debug
mingw32-make.exe
@popd


@echo Done
@pause