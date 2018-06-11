@echo off

@pushd ..
    @call cmake_configure CMake "%CD%\..\CMake\SDL32" "..\vs12_solution_sdl32_debug" "Visual Studio 12 2013" "" "-DCMAKE_CONFIGURATION_TYPES:STRING='Debug'" "-DMENGINE_LIB_DIR:STRING='build_msvc12_sdl32_debug'" "-DCMAKE_BUILD_TYPE:STRING='Debug'"
@popd

@pause
@echo on
