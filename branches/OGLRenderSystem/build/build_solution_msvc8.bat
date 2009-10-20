@echo off

@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\src" "..\vs8_solution" "Visual Studio 8 2005" "" "-DCMAKE_CONFIGURATION_TYPES:STRING='Debug;Release'" "-DMENGINE_LIB_DIR:STRING='build_msvc8'"

::"..\dependencies\cmake\bin\cmake.exe" -DCMAKE_CONFIGURATION_TYPES:STRING="Debug;Release" -G"Visual Studio 8 2005" -DMENGINE_LIB_DIR:STRING="build_msvc8" ../src
@pause
@echo on
