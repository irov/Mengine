@echo off

@pushd ..
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\SDL" "..\vs12_solution_sdl_debug" "Visual Studio 12" "" "-DCMAKE_CONFIGURATION_TYPES:STRING='Debug'" "-DMENGINE_LIB_DIR:STRING='build_sdl'" "-DCMAKE_BUILD_TYPE:STRING='Debug'"
@popd

@pause
@echo on
