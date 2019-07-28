@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting build solution msvc %CONFIGURATION% configuration...

set "VERSION=16"
set "YEAR=2019"

@pushd %~dp0..
@call vcvarsall_msvc%VERSION%.bat
@popd

@pushd %~dp0..
@call build_solution.bat "SOLUTION_NAME=solution_msvc%VERSION%" "SOURCE_DIRECTORY=%CD%\..\CMake\Win32" "GENERATOR=Visual Studio %VERSION% %YEAR%" "CONFIGURATION=%CONFIGURATION%" "ARCHITECTURE=Win32" "LIBRARY_DIRECTORY=Depends_WIN32"
@popd

:end
@echo Done

@pause