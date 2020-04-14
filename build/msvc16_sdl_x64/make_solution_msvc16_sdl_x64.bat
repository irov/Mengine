@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting make solution SDL %CONFIGURATION% configuration...

set "VERSION=16"
set "YEAR=2019"

@pushd %~dp0..
@call vcvarsall_msvc%VERSION%.bat
@popd

@pushd %~dp0..
@call make_solution.bat "SOLUTION_NAME=solution_msvc%VERSION%_sdl_x64" "SOURCE_DIRECTORY=%CD%\..\cmake\Win64_SDL" "GENERATOR=Visual Studio %VERSION% %YEAR%" "CONFIGURATION=%CONFIGURATION%" "ARCHITECTURE=x64" "LIBRARY_DIRECTORY=Depends_WIN64_SDL"
@popd

:end
@echo Done

@pause