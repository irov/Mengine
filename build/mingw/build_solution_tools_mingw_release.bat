@echo off

@pushd ..
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\Tools_MinGW" "%CD%\..\build_temp\build_mingw_tools" "MinGW Makefiles" Release "-DMENGINE_LIB_DIR:STRING='build_mingw_dependencies'"
@popd

@pushd ..\..\build_temp\build_mingw_tools\Release
mingw32-make.exe
@popd

@if NOT "%1"=="/NOPAUSE" pause
@echo on
