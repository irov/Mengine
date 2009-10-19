@echo off

@echo Starting dependencies build release configuration...
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\dependencies\cmake_scripts" "..\dependencies\build_msvc8" "NMake Makefiles" Release
@pushd ..\dependencies\build_msvc8_Release
nmake
@popd

@echo Done
@pause