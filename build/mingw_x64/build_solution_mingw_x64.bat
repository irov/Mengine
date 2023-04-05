@echo off

set "SOLUTION_NAME=solution_mingw64"
set "SOURCE_DIRECTORY=%~dp0..\..\cmake\MinGW64"
set "GENERATOR=Ninja"
set "BUILD_PUBLISH=OFF"

@echo Starting build %SOLUTION_NAME% configuration...

@pushd %~dp0..
@call build_solution.bat %* "SOLUTION_NAME=%SOLUTION_NAME%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%" "BUILD_PUBLISH=%BUILD_PUBLISH%"
@popd

:end

exit /b %errorlevel%