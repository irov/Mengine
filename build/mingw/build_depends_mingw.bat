@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting build dependencies mingw %CONFIGURATION% configuration...

@pushd %~dp0..
@call build_depends.bat "SOLUTION_NAME=dependencies_mingw" "SOURCE_DIRECTORY=%CD%\..\cmake\Depends_MinGW" "GENERATOR=Ninja" "CONFIGURATION=%CONFIGURATION%"
@popd

:end

exit /b %errorlevel%