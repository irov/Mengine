@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting build dependencies %CONFIGURATION% configuration...

@pushd %~dp0..
@call vcvarsall_msvc15.bat
@popd

@pushd %~dp0..
@call build_depends.bat "%CD%\..\CMake\Depends_WIN32" build_msvc15\%CONFIGURATION% "Visual Studio 15 2017" %CONFIGURATION%
@popd

:end
@echo Done

@pause