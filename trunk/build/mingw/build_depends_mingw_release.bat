@echo off

@echo Starting dependencies build...

@pushd ..
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\Depends_MinGW" "..\dependencies\build_mingw" "MinGW Makefiles" Release
@popd

@pushd ..\..\dependencies\build_mingw\Release
mingw32-make.exe
@popd


@echo Done
@pause