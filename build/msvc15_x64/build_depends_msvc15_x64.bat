@echo off

set "CONFIGURATION=%1"

@echo Starting build dependencies x64 %CONFIGURATION% configuration...

@pushd ..
@call vcvarsall_msvc15_x64.bat
@popd

@pushd ..
@call build_depends.bat "%CD%\..\CMake\Depends_WIN64" build_msvc15_x64\%CONFIGURATION% "Visual Studio 15 2017 Win64" %CONFIGURATION%
@popd

@echo Done

@pause