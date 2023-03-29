@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"
set "EXTERNAL_PDB_PATH=%2"
set "BUILD_NUMBER=%3"
set "BUILD_VERSION=%4"

set "SOLUTION_NAME=solution_mingw_publish"
set "SOURCE_DIRECTORY=%CD%\..\cmake\MinGW"
set "GENERATOR=Ninja"
set "BUILD_PUBLISH=ON"

@echo Starting build %SOLUTION_NAME% %CONFIGURATION% configuration...

@pushd %~dp0..
@call build_solution.bat "SOLUTION_NAME=%SOLUTION_NAME%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%" "BUILD_PUBLISH=%BUILD_PUBLISH%" "CONFIGURATION=%CONFIGURATION%" "EXTERNAL_PDB_PATH=%EXTERNAL_PDB_PATH%" "BUILD_NUMBER=%BUILD_NUMBER%" "BUILD_VERSION=%BUILD_VERSION%"
@popd

:end

exit /b %errorlevel%