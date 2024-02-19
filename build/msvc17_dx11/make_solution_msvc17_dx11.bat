@echo off

set "VERSION=17"
set "YEAR=2022"

set "SOLUTION_NAME=solution_msvc%VERSION%_dx11"
set "SOURCE_DIRECTORY=%~dp0..\..\cmake\Win32_DX11"
set "GENERATOR=Visual Studio %VERSION% %YEAR%"
set "ARCHITECTURE=Win32"
set "BUILD_PUBLISH=OFF"

@call %~dp0../vcvarsall_msvc%VERSION%.bat

@echo Starting make %SOLUTION_NAME% configuration...

@call %~dp0../make_solution.bat %* "SOLUTION_NAME=%SOLUTION_NAME%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%" "BUILD_PUBLISH=%BUILD_PUBLISH%" "ARCHITECTURE=%ARCHITECTURE%"

@exit /b %errorlevel%