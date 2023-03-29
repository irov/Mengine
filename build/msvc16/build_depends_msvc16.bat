@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting build dependencies msvc %CONFIGURATION% configuration...

set "VERSION=16"
set "YEAR=2019"

@pushd %~dp0..
@call vcvarsall_msvc%VERSION%.bat
@popd

@pushd %~dp0..
@call build_depends.bat "SOLUTION_NAME=dependencies_msvc%VERSION%" "SOURCE_DIRECTORY=%CD%\..\cmake\Depends_WIN32" "GENERATOR=Visual Studio %VERSION% %YEAR%" "CONFIGURATION=%CONFIGURATION%" "VERBOSITY=minimal" "ARCHITECTURE=Win32"
@popd

:end

@exit /b %errorlevel%