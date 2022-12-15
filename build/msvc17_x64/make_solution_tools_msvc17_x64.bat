@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting make solution tools %CONFIGURATION% configuration...

set "VERSION=17"
set "YEAR=2022"

@pushd %~dp0..
@call vcvarsall_msvc%VERSION%_x64.bat
@popd

@pushd %~dp0..
@call make_solution.bat "SOLUTION_NAME=solution_tools_msvc%VERSION%_x64" "SOURCE_DIRECTORY=%CD%\..\cmake\Tools_Win64" "GENERATOR=Visual Studio %VERSION% %YEAR%" "CONFIGURATION=%CONFIGURATION%" "ARCHITECTURE=x64"
@popd

:end

@pause
exit /b %errorlevel%