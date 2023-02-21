@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

if ["%~2"]==[""] (
  @echo invalid arguments, please set deploy path
  goto end
)

set "CONFIGURATION=%1"
set "DEPLOY_PATH=%2"
set "EXTERNAL_PDB_PATH=%3"
set "BUILD_VERSION=%4"

@echo Starting make solution %CONFIGURATION% configuration...

set "VERSION=17"
set "YEAR=2022"

@pushd %~dp0..
@call vcvarsall_msvc%VERSION%_x64.bat
@popd

@pushd %~dp0..
@call make_solution.bat "SOLUTION_NAME=solution_msvc%VERSION%_uwp" "SOURCE_DIRECTORY=%CD%\..\cmake\Win32_UWP" "GENERATOR=Visual Studio %VERSION% %YEAR%" "CONFIGURATION=%CONFIGURATION%" "ARCHITECTURE=x64" "DEPLOY_PATH=%DEPLOY_PATH%" "EXTERNAL_PDB_PATH=%EXTERNAL_PDB_PATH%" "BUILD_VERSION=%BUILD_VERSION%"
@popd

:end

@pause
exit /b %errorlevel%