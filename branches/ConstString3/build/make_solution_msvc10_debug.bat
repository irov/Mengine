@echo off

@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\Win32" "..\vs10_solution_debug" "Visual Studio 10" "" "-DCMAKE_CONFIGURATION_TYPES:STRING='Debug'" "-DMENGINE_LIB_DIR:STRING='build_msvc10'" "-DCMAKE_BUILD_TYPE:STRING='Debug'"

@pause
@echo on
