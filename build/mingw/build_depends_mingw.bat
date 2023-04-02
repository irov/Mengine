@echo off

set "SOLUTION_NAME=dependencies_mingw"
set "SOURCE_DIRECTORY=%CD%\..\..\cmake\Depends_MinGW"
set "GENERATOR=Ninja"

@echo Starting build %SOLUTION_NAME% configuration...

@pushd %~dp0..
@call build_depends.bat %* "SOLUTION_NAME=%SOLUTION_NAME%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%"
@popd

:end

exit /b %errorlevel%