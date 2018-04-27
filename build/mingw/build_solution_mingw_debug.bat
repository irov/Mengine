@echo off

@pushd ..
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\MinGW" "%CD%\..\build_temp\build_mingw_application" "MinGW Makefiles" Debug "-DMENGINE_LIB_DIR:STRING='build_mingw_dependencies'"
@popd

@pushd ..\..\build_temp\build_mingw_application\Debug
mingw32-make.exe
@popd

@if NOT "%1"=="/NOPAUSE" pause
@echo on
