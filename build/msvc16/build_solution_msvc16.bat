@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting build solution %CONFIGURATION% configuration...

set "VERSION=16"
set "YEAR=2019"

@pushd %~dp0..
@call vcvarsall_msvc%VERSION%.bat
@popd

@pushd %~dp0..
@call build_solution.bat "%CD%\..\CMake\Win32" solution_msvc%VERSION%\%CONFIGURATION% "Visual Studio %VERSION% %YEAR%" %CONFIGURATION% build_msvc%VERSION%\%CONFIGURATION% /verbosity:minimal
@popd

:end
@echo Done