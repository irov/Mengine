@echo off

@pushd ..
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\MinGW" "%CD%\..\build_temp\build_mingw_application_release" "MinGW Makefiles" Release "-DMENGINE_LIB_DIR:STRING='build_mingw_dependencies_release'"
@popd

@pushd ..\..\build_temp\build_mingw_application_release\Release
mingw32-make.exe
@popd

@if NOT "%1"=="/NOPAUSE" pause
@echo on
