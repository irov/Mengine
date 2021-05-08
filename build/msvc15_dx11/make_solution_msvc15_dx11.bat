@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting make solution %CONFIGURATION% configuration...

set "VERSION=15"
set "YEAR=2017"

@pushd %~dp0..
@call vcvarsall_msvc%VERSION%.bat
@popd

@pushd %~dp0..
@call make_solution.bat "SOLUTION_NAME=solution_msvc%VERSION%_dx11" "SOURCE_DIRECTORY=%CD%\..\cmake\Win32_DX11" "GENERATOR=Visual Studio %VERSION% %YEAR%" "CONFIGURATION=%CONFIGURATION%"
@popd

:end
@echo Done

@pause