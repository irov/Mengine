@echo off

set "CONFIGURATION=%1"

@echo Starting build solution mingw %CONFIGURATION% configuration...

@pushd ..
@call cmake_configure CMake "%CD%\..\CMake\MinGW" "%CD%\..\solutions\solution_mingw\%CONFIGURATION%" "MinGW Makefiles" %CONFIGURATION% "-DMENGINE_LIB_DIR:STRING='build_mingw\%CONFIGURATION%'"
@popd

@pushd "%CD%\..\solutions\solution_mingw\%CONFIGURATION%"
@call mingw32-make.exe
@popd

@echo Done

@pause