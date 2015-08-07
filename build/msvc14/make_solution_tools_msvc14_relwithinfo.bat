@echo off

@pushd ..
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\Tools_Win32" "..\vs14_solution_tools_relwithinfo" "Visual Studio 14" "" "-DCMAKE_CONFIGURATION_TYPES:STRING='RelWithDebInfo'" "-DMENGINE_LIB_DIR:STRING='build_msvc14'" "-DCMAKE_BUILD_TYPE:STRING='RelWithDebInfo'"
@popd

@pause
@echo on
