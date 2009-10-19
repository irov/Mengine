@echo off

@echo Starting dependencies build debug configuration...
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\dependencies\cmake_scripts" "..\dependencies\build_msvc8" "NMake Makefiles" Debug
@pushd ..\dependencies\build_msvc8_Debug
nmake
@popd

@echo Done
@pause