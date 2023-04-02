@echo off

set "VERSION=17"
set "YEAR=2022"

set "SOLUTION_NAME=solution_msvc%VERSION%"
set "SOURCE_DIRECTORY=%CD%\..\..\cmake\Win32"
set "GENERATOR=Visual Studio %VERSION% %YEAR%"
set "ARCHITECTURE=Win32"
set "BUILD_PUBLISH=OFF"

@pushd %~dp0..
@call vcvarsall_msvc%VERSION%.bat
@popd

@echo Starting make %SOLUTION_NAME% configuration...

@pushd %~dp0..
@call make_solution.bat %* "SOLUTION_NAME=%SOLUTION_NAME%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%" "BUILD_PUBLISH=%BUILD_PUBLISH%" "ARCHITECTURE=%ARCHITECTURE%"
@popd

:end

@exit /b %errorlevel%