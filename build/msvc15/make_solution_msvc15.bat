@echo off

set "VERSION=15"
set "YEAR=2017"

set "SOLUTION_NAME=solution_msvc%VERSION%"
set "SOURCE_DIRECTORY=%~dp0..\..\cmake\Win32"
set "GENERATOR=Visual Studio %VERSION% %YEAR%"
set "ARCHITECTURE=Win32"
set "BUILD_PUBLISH=OFF"

@pushd %~dp0..
@call vcvarsall_msvc%VERSION%.bat
@popd

@echo Starting make %SOLUTION_NAME% configuration...

@pushd %~dp0..
@call make_solution.bat "SOLUTION_NAME=%SOLUTION_NAME%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%" "BUILD_PUBLISH=%BUILD_PUBLISH%" "ARCHITECTURE=%ARCHITECTURE%"
@popd

:end

@exit /b %errorlevel%