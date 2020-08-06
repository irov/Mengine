@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"
set "EXTERNAL_PDB_PATH=%2"
set "BUILD_VERSION=%3"
set "RESOURCE_GIT_SHA1=%4"

@echo Starting build solution msvc %CONFIGURATION% configuration...

set "VERSION=15"
set "YEAR=2017"

@pushd %~dp0..
@call vcvarsall_msvc%VERSION%.bat
@popd

@pushd %~dp0..
@call build_solution.bat "SOLUTION_NAME=solution_msvc%VERSION%" "SOURCE_DIRECTORY=%CD%\..\cmake\Win32" "GENERATOR=Visual Studio %VERSION% %YEAR%" "CONFIGURATION=%CONFIGURATION%" "EXTERNAL_PDB_PATH=%EXTERNAL_PDB_PATH%" "BUILD_VERSION=%BUILD_VERSION%" "RESOURCE_GIT_SHA1=%RESOURCE_GIT_SHA1%"
@popd

:end
@echo Done

@pause
