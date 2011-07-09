@echo off

@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\src" "..\vs10_solution" "Visual Studio 10" "" "-DCMAKE_CONFIGURATION_TYPES:STRING='Debug;Release;RelWithDebInfo'" "-DMENGINE_LIB_DIR:STRING='build_msvc10'"

::"..\dependencies\cmake\bin\cmake.exe" -DCMAKE_CONFIGURATION_TYPES:STRING="Debug;Release" -G"Visual Studio 10 2010" -DMENGINE_LIB_DIR:STRING="build_msvc8" ../src
@pause
@echo on
