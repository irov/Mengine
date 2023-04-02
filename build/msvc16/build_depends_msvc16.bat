@echo off

set "VERSION=16"
set "YEAR=2019"

set "SOLUTION_NAME=dependencies_msvc%VERSION%"
set "SOURCE_DIRECTORY=%CD%\..\..\cmake\Depends_WIN32"
set "GENERATOR=Visual Studio %VERSION% %YEAR%"
set "ARCHITECTURE=Win32"

@echo Starting build %SOLUTION_NAME% configuration...

@pushd %~dp0..
@call vcvarsall_msvc%VERSION%.bat
@popd

@pushd %~dp0..
@call build_depends.bat %* "SOLUTION_NAME=%SOLUTION_NAME%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%"  "ARCHITECTURE=%ARCHITECTURE%" "VERBOSITY=minimal"
@popd

:end

@exit /b %errorlevel%