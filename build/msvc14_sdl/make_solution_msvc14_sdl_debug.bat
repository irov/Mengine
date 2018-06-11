@echo off

@pushd ..
    @call cmake_configure CMake "%CD%\..\CMake\SDL32" "..\vs14_solution_sdl32_debug" "Visual Studio 14 2015" "" "-DCMAKE_CONFIGURATION_TYPES:STRING='Debug'" "-DMENGINE_LIB_DIR:STRING='build_temp/build_msvc14_sdl32_debug'" "-DCMAKE_BUILD_TYPE:STRING='Debug'"
@popd

@pause
@echo on
