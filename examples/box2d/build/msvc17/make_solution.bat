@echo off

set "VERSION=17"
set "YEAR=2022"

set "SOLUTION_NAME=solution_msvc%VERSION%"
set "SOLUTION_BASE_DIR=%~dp0..\..\solutions"
set "SOURCE_DIRECTORY=%~dp0..\..\cmake\Win32"
set "DEPENDENCIES_PROJECT=Depends_WIN32"
set "GENERATOR=Visual Studio %VERSION% %YEAR%"
set "ARCHITECTURE=Win32"
set "BUILD_PUBLISH=OFF"

@call %~dp0../vcvarsall_msvc%VERSION%.bat

@echo Starting make %SOLUTION_NAME% configuration...

@call %~dp0../../../../build/make_solution.bat %* "SOLUTION_NAME=%SOLUTION_NAME%" "DEPENDENCIES_PROJECT=%DEPENDENCIES_PROJECT%" "SOLUTION_BASE_DIR=%SOLUTION_BASE_DIR%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%" "BUILD_PUBLISH=%BUILD_PUBLISH%" "ARCHITECTURE=%ARCHITECTURE%"

@exit /b %errorlevel%

@echo off