@echo off

set "VERSION=17"
set "YEAR=2022"

set "SOLUTION_NAME=solution_msvc%VERSION%_x64"
set "SOURCE_DIRECTORY=%~dp0..\..\cmake\Win64"
set "DEPENDENCIES_PROJECT=Depends_WIN64"
set "GENERATOR=Visual Studio %VERSION% %YEAR%"
set "ARCHITECTURE=x64"

@call %~dp0../vcvarsall_msvc%VERSION%_x64.bat

@call %~dp0../make_solution.bat %* "SOLUTION_NAME=%SOLUTION_NAME%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%" "ARCHITECTURE=%ARCHITECTURE%"

@exit /b %errorlevel%