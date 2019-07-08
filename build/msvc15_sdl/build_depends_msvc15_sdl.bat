@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting build dependencies SDL %CONFIGURATION% configuration...

set "VERSION=15"
set "YEAR=2017"

@pushd %~dp0..
@call vcvarsall_msvc%VERSION%.bat
@popd

@pushd %~dp0..
@call build_depends.bat "SOLUTION_NAME=build_msvc%VERSION%_sdl" "CONFIGURATION=%CONFIGURATION%" "SOURCE_DIRECTORY=%CD%\..\CMake\Depends_WIN32_SDL" "GENERATOR=Visual Studio %VERSION% %YEAR%" "VERBOSITY=minimal"
@popd

:end
@echo Done