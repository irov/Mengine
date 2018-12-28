@echo off

set "CONFIGURATION=%1"

@echo Starting make solution tools x64 %CONFIGURATION% configuration...

@pushd ..
@call vcvarsall_msvc15_x64.bat
@popd

@pushd ..
@call make_solution.bat "%CD%\..\CMake\Tools_Win64" solution_tools_msvc15_x64\%CONFIGURATION% "Visual Studio 15 2017 Win64" %CONFIGURATION% build_msvc15_x64\%CONFIGURATION%
@popd

@echo Done

@pause