@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"
set "EXTERNAL_PDB_PATH=%2"
set "BUILD_VERSION=%3"

@echo Starting make solution %CONFIGURATION% configuration...

set "VERSION=16"
set "YEAR=2019"

@pushd %~dp0..
@call vcvarsall_msvc%VERSION%.bat
@popd

@pushd %~dp0..
@call make_solution.bat "SOLUTION_NAME=solution_msvc%VERSION%_dx11" "SOURCE_DIRECTORY=%CD%\..\cmake\Win32_DX11" "GENERATOR=Visual Studio %VERSION% %YEAR%" "CONFIGURATION=%CONFIGURATION%" "ARCHITECTURE=Win32" "EXTERNAL_PDB_PATH=%EXTERNAL_PDB_PATH%" "BUILD_VERSION=%BUILD_VERSION%"
@popd

:end

@pause
@exit /b %errorlevel%