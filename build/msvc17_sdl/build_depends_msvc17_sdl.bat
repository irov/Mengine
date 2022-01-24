@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting build dependencies msvc SDL %CONFIGURATION% configuration...

set "VERSION=17"
set "YEAR=2022"

@pushd %~dp0..
@call vcvarsall_msvc%VERSION%.bat
@popd

@pushd %~dp0..
@call build_depends.bat "SOLUTION_NAME=dependencies_msvc%VERSION%_sdl" "SOURCE_DIRECTORY=%CD%\..\cmake\Depends_WIN32_SDL" "GENERATOR=Visual Studio %VERSION% %YEAR%" "CONFIGURATION=%CONFIGURATION%" "VERBOSITY=minimal" "ARCHITECTURE=Win32"
@popd

:end

@pause
exit /b %errorlevel%