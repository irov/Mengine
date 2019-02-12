@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting make solution %CONFIGURATION% configuration...

@pushd ..
@call vcvarsall_msvc15.bat
@popd

@pushd ..
@call make_solution.bat "%CD%\..\CMake\Win32" solution_msvc15_master\%CONFIGURATION% "Visual Studio 15 2017" %CONFIGURATION% build_msvc15\%CONFIGURATION% "-DMENGINE_PROJECT_NAME:STRING=Mengine_WIN32_MASTER" "-DMENGINE_BUILD_MENGINE_MASTER_RELEASE:BOOLEAN=ON"
@popd

:end
@echo on

@pause
