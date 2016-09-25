@echo off

@pushd ..
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\Tools_Win32" "..\vs14_solution_tools_release" "Visual Studio 14" "" "-DCMAKE_CONFIGURATION_TYPES:STRING='Release'" "-DMENGINE_LIB_DIR:STRING='build_msvc14_release'" "-DCMAKE_BUILD_TYPE:STRING='Release'"
@popd

@pause
@echo on
