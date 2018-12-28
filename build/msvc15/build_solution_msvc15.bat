@echo off

set "CONFIGURATION=%1"

@echo Starting build solution %CONFIGURATION% configuration...

@pushd ..
@call vcvarsall_msvc15.bat
@popd

@pushd ..
@call build_solution.bat "%CD%\..\CMake\Win32" solution_msvc15\%CONFIGURATION% "Visual Studio 15 2017" %CONFIGURATION% build_msvc15\%CONFIGURATION%
@popd

@echo Done

@pause
