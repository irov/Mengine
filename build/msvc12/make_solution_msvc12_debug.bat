@echo off

@pushd ..
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\Win32" "..\vs12_solution_debug" "Visual Studio 12 2012" "" "-DCMAKE_CONFIGURATION_TYPES:STRING='Debug'" "-DMENGINE_LIB_DIR:STRING='build_msvc12'" "-DCMAKE_BUILD_TYPE:STRING='Debug'"
@popd

@pause
@echo on
