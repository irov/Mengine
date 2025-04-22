@echo off

set "VERSION=17"
set "YEAR=2022"

set "SOLUTION_NAME=solution_msvc%VERSION%_uwp"
set "SOURCE_DIRECTORY=%~dp0..\..\cmake\UWP"
set "DEPENDENCIES_PROJECT=Depends_WIN32"
set "GENERATOR=Visual Studio %VERSION% %YEAR%"
set "ARCHITECTURE=x64"
set "BUILD_PUBLISH=OFF"

set "DEPLOY_PATH=%2"
set "EXTERNAL_PDB_PATH=%3"
set "BUILD_VERSION=%4"

@call %~dp0../vcvarsall_msvc%VERSION%_x64.bat

@echo Starting make %SOLUTION_NAME% configuration...

@call %~dp0../make_solution.bat %* "SOLUTION_NAME=%SOLUTION_NAME%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%" "BUILD_PUBLISH=%BUILD_PUBLISH%" "ARCHITECTURE=%ARCHITECTURE%"

exit /b %errorlevel%