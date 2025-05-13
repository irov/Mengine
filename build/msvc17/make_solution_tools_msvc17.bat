@echo off

set "VERSION=17"
set "YEAR=2022"

set "SOLUTION_NAME=solution_tools_msvc%VERSION%"
set "SOURCE_DIRECTORY=%~dp0..\..\cmake\Tools_Win32"
set "DEPENDENCIES_PROJECT=Depends_WIN32"
set "GENERATOR=Visual Studio %VERSION% %YEAR%"
set "ARCHITECTURE=Win32"

@call %~dp0../vcvarsall_msvc%VERSION%.bat

@echo Starting make %SOLUTION_NAME% configuration...

@call %~dp0../make_solution.bat %* "SOLUTION_NAME=%SOLUTION_NAME%" "DEPENDENCIES_PROJECT=%DEPENDENCIES_PROJECT%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%" "ARCHITECTURE=%ARCHITECTURE%"

@exit /b %errorlevel%