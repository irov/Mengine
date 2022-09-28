@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"
set "EXTERNAL_PDB_PATH=%2"
set "BUILD_VERSION=%3"

@echo Starting build solution mingw64 %CONFIGURATION% configuration...

@pushd %~dp0..
@call build_solution.bat "SOLUTION_NAME=solution_mingw64" "SOURCE_DIRECTORY=%CD%\..\cmake\MinGW64" "GENERATOR=Ninja" "BUILD_PUBLISH=ON" "CONFIGURATION=%CONFIGURATION%" "EXTERNAL_PDB_PATH=%EXTERNAL_PDB_PATH%" "BUILD_VERSION=%BUILD_VERSION%"
@popd

:end

@pause
exit /b %errorlevel%