@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

set "SOLUTION_NAME=solution_tools_mingw64"

@echo Starting build %SOLUTION_NAME% %CONFIGURATION% configuration...

@pushd %~dp0..
@call build_solution.bat "SOLUTION_NAME=%SOLUTION_NAME%" "CONFIGURATION=%CONFIGURATION%"
@popd

:end

exit /b %errorlevel%