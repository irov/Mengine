@echo off

set "VERSION=17"
set "YEAR=2022"

set "SOLUTION_NAME=solution_msvc%VERSION%"
set "SOURCE_DIRECTORY=%~dp0..\..\cmake\Win32"
set "GENERATOR=Visual Studio %VERSION% %YEAR%"
set "ARCHITECTURE=Win32"

@call %~dp0../vcvarsall_msvc%VERSION%.bat

@call %~dp0../make_solution.bat %* "SOLUTION_NAME=%SOLUTION_NAME%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%" "ARCHITECTURE=%ARCHITECTURE%"

@exit /b %errorlevel%