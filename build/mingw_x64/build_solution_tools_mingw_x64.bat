@echo off

set "SOLUTION_NAME=solution_tools_mingw64"
set "SOURCE_DIRECTORY=%~dp0..\..\cmake\Tools_MinGW64"
set "DEPENDENCIES_PROJECT=Depends_MINGW64"
set "GENERATOR=Ninja"

@echo Starting build %SOLUTION_NAME% %CONFIGURATION% configuration...

@call %~dp0../build_solution.bat %* "SOLUTION_NAME=%SOLUTION_NAME%" "DEPENDENCIES_PROJECT=%DEPENDENCIES_PROJECT%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%"

exit /b %errorlevel%