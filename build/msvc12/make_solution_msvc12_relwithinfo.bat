@echo off

@pushd ..
@call cmake_configure CMake "%CD%\..\CMake\Win32" "..\vs12_solution_relwithinfo" "Visual Studio 12 2013" "" "-DCMAKE_CONFIGURATION_TYPES:STRING='RelWithDebInfo'" "-DMENGINE_LIB_DIR:STRING='build_msvc12'" "-DCMAKE_BUILD_TYPE:STRING='RelWithDebInfo'"
@popd

@pause
@echo on
