@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting build solution mingw64 %CONFIGURATION% configuration...

@pushd %~dp0..
@call build_solution.bat "SOLUTION_NAME=solution_mingw64" "SOURCE_DIRECTORY=%CD%\..\cmake\MinGW64" "GENERATOR=MinGW Makefiles" "CONFIGURATION=%CONFIGURATION%"
@popd

:end
@echo Done

@pause