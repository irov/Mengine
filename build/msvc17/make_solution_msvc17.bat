@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"
set "EXTERNAL_PDB_PATH=%2"
set "BUILD_NUMBER=%3"
set "BUILD_VERSION=%4"

set "VERSION=17"
set "YEAR=2022"

set "SOLUTION_NAME=solution_msvc%VERSION%"
set "SOURCE_DIRECTORY=%CD%\..\cmake\Win32"
set "GENERATOR=Visual Studio %VERSION% %YEAR%"
set "ARCHITECTURE=Win32"
set "BUILD_PUBLISH=OFF"

@pushd %~dp0..
@call vcvarsall_msvc%VERSION%.bat
@popd

@echo Starting make %SOLUTION_NAME% %CONFIGURATION% configuration...

@pushd %~dp0..
@call make_solution.bat "SOLUTION_NAME=%SOLUTION_NAME%" "SOURCE_DIRECTORY=%SOURCE_DIRECTORY%" "GENERATOR=%GENERATOR%" "BUILD_PUBLISH=%BUILD_PUBLISH%" "CONFIGURATION=%CONFIGURATION%" "ARCHITECTURE=%ARCHITECTURE%" "EXTERNAL_PDB_PATH=%EXTERNAL_PDB_PATH%" "BUILD_NUMBER=%BUILD_NUMBER%" "BUILD_VERSION=%BUILD_VERSION%"
@popd

:end

@exit /b %errorlevel%