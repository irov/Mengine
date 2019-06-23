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
@call make_solution.bat "%CD%\..\CMake\Tools_Win32" solution_tools_msvc%VERSION%\%CONFIGURATION% "Visual Studio %VERSION% %YEAR%" %CONFIGURATION% build_msvc%VERSION%\%CONFIGURATION%
@popd

:end
@echo Done

@pause

