@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting make solution x64 %CONFIGURATION% configuration...

@pushd ..
@call vcvarsall_msvc15_x64.bat
@popd

@pushd ..
@call make_solution.bat "%CD%\..\cmake\Win64_SDL" solution_msvc15_x64_sdl\%CONFIGURATION% "Visual Studio 15 2017 Win64" %CONFIGURATION% build_msvc15_x64_sdl\%CONFIGURATION%
@popd

:done
@echo Done

@pause
