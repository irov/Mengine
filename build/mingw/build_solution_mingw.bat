@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting build solution mingw %CONFIGURATION% configuration...

@pushd %~dp0..
@call build_solution.bat "SOURCE_DIRECTORY=%CD%\..\CMake\MinGW" "SOLUTION_NAME=solution_mingw" "GENERATOR=MinGW Makefiles" "CONFIGURATION=%CONFIGURATION%" "LIBRARY_DIRECTORY=build_mingw"
@popd

:end
@echo Done

@pause