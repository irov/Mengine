@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting make solution tools %CONFIGURATION% configuration...

set "VERSION=16"
set "YEAR=2019"

@pushd %~dp0..
@call vcvarsall_msvc%VERSION%.bat
@popd

@pushd %~dp0..
@call make_solution.bat "SOLUTION_NAME=solution_tools_msvc%VERSION%" "SOURCE_DIRECTORY=%CD%\..\cmake\Tools_Win32" "GENERATOR=Visual Studio %VERSION% %YEAR%" "CONFIGURATION=%CONFIGURATION%" "ARCHITECTURE=Win32"
@popd

:end

exit /b %errorlevel%