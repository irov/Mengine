@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting build solution mingw %CONFIGURATION% configuration...

@pushd ..
@call cmake_configure CMake "%CD%\..\CMake\MinGW" "%CD%\..\solutions\solution_mingw\%CONFIGURATION%" "MinGW Makefiles" %CONFIGURATION% "-DMENGINE_LIB_DIR:STRING='build_mingw\%CONFIGURATION%'"
@popd

@pushd "%CD%\..\solutions\solution_mingw\%CONFIGURATION%"
@call mingw32-make.exe
@popd

:end
@echo Done

@pause