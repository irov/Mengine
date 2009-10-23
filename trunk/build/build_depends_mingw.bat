@echo off

@echo Starting dependencies build...

@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\dependencies\cmake_scripts" "..\dependencies\build_mingw" "MinGW Makefiles" Release
@pushd ..\dependencies\build_mingw_Release
mingw32-make.exe
@popd


@echo Done
@pause