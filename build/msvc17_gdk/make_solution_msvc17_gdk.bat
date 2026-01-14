@echo off

set "VERSION=17"
set "YEAR=2022"

set "SOLUTION_NAME=solution_msvc%VERSION%_gdk"
set "SOURCE_DIRECTORY=%~dp0..\..\cmake\GDK"
set "DEPENDENCIES_PROJECT=Depends_GDK"
set "GENERATOR=Visual Studio %VERSION% %YEAR%"
set "ARCHITECTURE=x64"

@call %~dp0../vcvarsall_msvc%VERSION%_x64.bat

@call %~dp0../make_solution.bat %* "SOLUTION_NAME=%SOLUTION_NAME%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%" "ARCHITECTURE=%ARCHITECTURE%"

@exit /b %errorlevel%
