@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting build solution tools mingw %CONFIGURATION% configuration...

@pushd %~dp0..
@call build_solution.bat "SOLUTION_NAME=solution_tools_mingw64" "SOURCE_DIRECTORY=%CD%\..\cmake\Tools_MinGW64" "GENERATOR=Ninja" "CONFIGURATION=%CONFIGURATION%"
@popd

if %errorlevel% NEQ 0 (
    goto end
)

@pushd "%CD%\..\solutions\solution_tools_mingw\%CONFIGURATION%"
@call mingw32-make.exe
@popd

:end

@pause
exit /b %errorlevel%