@echo off

set "VERSION=17"
set "YEAR=2022"

set "SOLUTION_NAME=dependencies_msvc%VERSION%_dx11_x64"
set "SOURCE_DIRECTORY=%~dp0..\..\cmake\Depends_WIN64_DX11"
set "GENERATOR=Visual Studio %VERSION% %YEAR%"
set "ARCHITECTURE=x64"

@call %~dp0../vcvarsall_msvc%VERSION%_x64.bat

@echo Starting build %SOLUTION_NAME% configuration...

@call %~dp0../build_depends.bat %* "SOLUTION_NAME=%SOLUTION_NAME%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%"  "ARCHITECTURE=%ARCHITECTURE%" "VERBOSITY=minimal"

@exit /b %errorlevel%