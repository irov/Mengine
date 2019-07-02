@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting make solution SDL %CONFIGURATION% configuration...

set "VERSION=15"
set "YEAR=2017"

@pushd %~dp0..
@call vcvarsall_msvc%VERSION%.bat
@popd

@pushd %~dp0..
@call make_solution.bat "SOURCE_DIRECTORY=%CD%\..\CMake\Win32_SDL" "SOLUTION_NAME=solution_msvc%VERSION%_sdl" "GENERATOR=Visual Studio %VERSION% %YEAR%" "CONFIGURATION=%CONFIGURATION%" "LIBRARY_DIRECTORY=build_msvc%VERSION%_sdl\%CONFIGURATION%"
@popd

:end
@echo Done

@pause
