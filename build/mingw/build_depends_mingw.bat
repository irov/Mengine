@echo off

set "SOLUTION_NAME=dependencies_mingw"
set "SOURCE_DIRECTORY=%~dp0..\..\cmake\Depends_MinGW"
set "GENERATOR=Ninja"

@echo Starting build %SOLUTION_NAME% configuration...

@call %~dp0../build_depends.bat %* "SOLUTION_NAME=%SOLUTION_NAME%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%"

exit /b %errorlevel%