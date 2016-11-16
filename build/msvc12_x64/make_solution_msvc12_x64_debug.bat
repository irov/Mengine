@echo off

@pushd ..
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\Win64" "..\vs12_solution_x64_debug" "Visual Studio 12 2013 Win64" "" "-DCMAKE_CONFIGURATION_TYPES:STRING='Debug'" "-DMENGINE_LIB_DIR:STRING='build_msvc12_x64_debug'" "-DCMAKE_BUILD_TYPE:STRING='Debug'"
@popd

@pause
@echo on
