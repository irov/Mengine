@echo off

set "CONFIGURATION=%1"

@echo Starting build dependencies mingw %CONFIGURATION% configuration...

@pushd ..
@call build_depends.bat "%CD%\..\CMake\Depends_MinGW" build_mingw\%CONFIGURATION% "MinGW Makefiles" %CONFIGURATION%
@popd

@echo Done

@pause