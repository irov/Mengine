@echo off

set "VERSION=17"
set "YEAR=2022"

set "SOLUTION_NAME=solution_tools_msvc%VERSION%_x64"
set "SOURCE_DIRECTORY=%~dp0..\..\cmake\Tools_Win64"
set "GENERATOR=Visual Studio %VERSION% %YEAR%"
set "ARCHITECTURE=x64"

@call %~dp0../vcvarsall_msvc%VERSION%_x64.bat

@echo Starting make %SOLUTION_NAME% configuration...

@call %~dp0../make_solution.bat %* "SOLUTION_NAME=%SOLUTION_NAME%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%" "ARCHITECTURE=%ARCHITECTURE%"

@exit /b %errorlevel%