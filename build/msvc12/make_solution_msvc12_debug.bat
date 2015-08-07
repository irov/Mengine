@echo off

@pushd ..
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\Win32" "..\vs14_solution_debug" "Visual Studio 14 2015" "" "-DCMAKE_CONFIGURATION_TYPES:STRING='Debug'" "-DMENGINE_LIB_DIR:STRING='build_msvc14'" "-DCMAKE_BUILD_TYPE:STRING='Debug'"
@popd

@pause
@echo on
