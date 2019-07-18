@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting build dependencies mingw %CONFIGURATION% configuration...

@pushd %~dp0..
@call build_depends.bat "SOLUTION_NAME=build_mingw" "CONFIGURATION=%CONFIGURATION%" "SOURCE_DIRECTORY=%CD%\..\CMake\Depends_MinGW" "GENERATOR=MinGW Makefiles"
@popd

:end
@echo Done

@pause