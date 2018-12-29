@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting build dependencies SDL %CONFIGURATION% configuration...

@pushd ..
@call vcvarsall_msvc15.bat
@popd

@pushd ..
@call build_depends.bat "%CD%\..\CMake\Depends_WIN32_SDL" build_msvc15_sdl\%CONFIGURATION% "Visual Studio 15 2017" %CONFIGURATION%
@popd

:end
@echo Done

@pause