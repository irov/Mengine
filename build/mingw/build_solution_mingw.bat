@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting build solution mingw %CONFIGURATION% configuration...

@pushd %~dp0..
@call build_solution.bat "SOLUTION_NAME=solution_mingw" "SOURCE_DIRECTORY=%CD%\..\cmake\MinGW" "GENERATOR=MinGW Makefiles" "CONFIGURATION=%CONFIGURATION%"
@popd

:end
@echo Done

@pause