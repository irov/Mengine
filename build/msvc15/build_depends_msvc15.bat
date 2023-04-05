@echo off

set "VERSION=15"
set "YEAR=2017"

set "SOLUTION_NAME=dependencies_msvc%VERSION%"
set "SOURCE_DIRECTORY=%~dp0..\..\cmake\Depends_WIN32"
set "GENERATOR=Visual Studio %VERSION% %YEAR%"

@echo Starting build %SOLUTION_NAME% configuration...

@pushd %~dp0..
@call vcvarsall_msvc%VERSION%.bat
@popd

@pushd %~dp0..
@call build_depends.bat %* "SOLUTION_NAME=%SOLUTION_NAME%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%" "VERBOSITY=minimal"
@popd

:end

@pause
@exit /b %errorlevel%