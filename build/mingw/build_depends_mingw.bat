@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting build dependencies mingw %CONFIGURATION% configuration...

@pushd %~dp0..
@call build_depends.bat "%CD%\..\CMake\Depends_MinGW" build_mingw\%CONFIGURATION% "MinGW Makefiles" %CONFIGURATION%
@popd

:end
@echo Done

@pause