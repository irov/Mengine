@echo off

set "SOLUTION_NAME=solution_tools_mingw"
set "SOURCE_DIRECTORY=%~dp0..\..\cmake\Tools_MinGW"
set "DEPENDENCIES_PROJECT=Depends_MINGW"
set "GENERATOR=Ninja"

@echo Starting build %SOLUTION_NAME% %CONFIGURATION% configuration...

@call %~dp0../build_solution.bat %* "SOLUTION_NAME=%SOLUTION_NAME%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%"

exit /b %errorlevel%