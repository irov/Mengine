@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting build solution tools mingw %CONFIGURATION% configuration...

@pushd %~dp0..
@call cmake_configure CMake "%CD%\..\..\cmake\Tools_MinGW" "%CD%\..\..\solutions\solution_tools_mingw\%CONFIGURATION%" "MinGW Makefiles" %CONFIGURATION%
@popd

@pushd "%~dp0..\solutions\solution_tools_mingw\%CONFIGURATION%"
@call mingw32-make.exe
@popd

:end

exit /b %errorlevel%