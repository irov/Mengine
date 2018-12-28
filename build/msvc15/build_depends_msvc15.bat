@echo off

set "CONFIGURATION=%1"

@echo Starting build dependencies %CONFIGURATION% configuration...

@pushd ..
@call vcvarsall_msvc15.bat
@popd

@pushd ..
@call build_depends.bat "%CD%\..\CMake\Depends_WIN32" build_msvc15\%CONFIGURATION% "Visual Studio 15 2017" %CONFIGURATION%
@popd

@echo Done

@pause