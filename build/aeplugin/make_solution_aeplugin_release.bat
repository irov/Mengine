@echo off

@pushd ..
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\AEPlugin" "..\vs12_solution_aeplugin_release" "Visual Studio 12 2013 Win64" "" "-DCMAKE_CONFIGURATION_TYPES:STRING='Release'" "-DMENGINE_LIB_DIR:STRING='build_aeplugin'" "-DCMAKE_BUILD_TYPE:STRING='Release'"
@popd

@pause
@echo on
