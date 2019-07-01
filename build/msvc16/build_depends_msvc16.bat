@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting build dependencies %CONFIGURATION% configuration...

set "VERSION=16"
set "YEAR=2019"

@pushd %~dp0..
@call vcvarsall_msvc%VERSION%.bat
@popd

@pushd %~dp0..
@call build_depends.bat "SOLUTION_NAME=build_msvc%VERSION%" "CONFIGURATION=%CONFIGURATION%" "SOURCE_DIRECTORY=%CD%\..\CMake\Depends_WIN32" "GENERATOR=Visual Studio %VERSION% %YEAR%" "VERBOSITY=minimal" "ARCHITECTURE=Win32"
@popd

:end
@echo Done