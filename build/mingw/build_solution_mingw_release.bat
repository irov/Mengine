@echo off

@pushd ..
@call cmake_configure CMake "%CD%\..\CMake\MinGW" "%CD%\..\build_temp\build_mingw_application" "MinGW Makefiles" Release "-DMENGINE_LIB_DIR:STRING='build_mingw_dependencies'"
@popd

@pushd ..\..\build_temp\build_mingw_application\Release
mingw32-make.exe -j4
@popd

@if NOT "%1"=="/NOPAUSE" pause
@echo on
