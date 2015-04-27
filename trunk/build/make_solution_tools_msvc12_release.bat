@echo off

@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\Tools_Win32" "..\vs12_solution_tools_release" "Visual Studio 12" "" "-DCMAKE_CONFIGURATION_TYPES:STRING='Release'" "-DMENGINE_LIB_DIR:STRING='build_msvc12'" "-DCMAKE_BUILD_TYPE:STRING='Release'"

@pause
@echo on
