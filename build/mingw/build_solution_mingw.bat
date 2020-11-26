@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"
set "EXTERNAL_PDB_PATH=%2"
set "BUILD_VERSION=%3"
set "RESOURCE_GIT_SHA1=%4"

@echo Starting build solution mingw %CONFIGURATION% configuration...

@pushd %~dp0..
@call build_solution.bat "SOLUTION_NAME=solution_mingw" "SOURCE_DIRECTORY=%CD%\..\cmake\MinGW" "GENERATOR=Ninja" "CONFIGURATION=%CONFIGURATION%" "EXTERNAL_PDB_PATH=%EXTERNAL_PDB_PATH%" "BUILD_VERSION=%BUILD_VERSION%" "RESOURCE_GIT_SHA1=%RESOURCE_GIT_SHA1%"
@popd

:end

@pause
exit /b %errorlevel%