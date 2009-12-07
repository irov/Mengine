@echo off

@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\src" "..\vs8_solution_dx9" "Visual Studio 8 2005" "" "-DCMAKE_CONFIGURATION_TYPES:STRING='Debug;Release;RelWithDebInfo'" "-DMENGINE_LIB_DIR:STRING='build_msvc8'" "-DMENGE_DX9:STRING=TRUE"
@pause
@echo on
