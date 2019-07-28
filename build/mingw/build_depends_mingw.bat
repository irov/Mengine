@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting build dependencies mingw %CONFIGURATION% configuration...

@pushd %~dp0..
@call build_depends.bat "SOLUTION_NAME=dependencies_mingw" "SOURCE_DIRECTORY=%CD%\..\CMake\Depends_MinGW" "GENERATOR=MinGW Makefiles" "CONFIGURATION=%CONFIGURATION%"
@popd

:end
@echo Done

@pause