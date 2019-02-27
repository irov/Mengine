@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting build solution mingw %CONFIGURATION% configuration...

@pushd %~dp0..
@call build_solution.bat "%CD%\..\CMake\MinGW" solution_mingw\%CONFIGURATION% "MinGW Makefiles" %CONFIGURATION% build_mingw
@popd

:end
@echo Done

@pause