@echo off

@echo Starting dependencies build debug configuration...
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\dependencies\cmake_scripts" "..\dependencies\build_msvc8" "NMake Makefiles" Debug
@pushd ..\dependencies\build_msvc8\Debug
nmake
@popd
@echo Starting dependencies build release configuration...
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\dependencies\cmake_scripts" "..\dependencies\build_msvc8" "NMake Makefiles" Release
@pushd ..\dependencies\build_msvc8\Release
nmake
@popd
@echo Starting dependencies build relwithdebinfo configuration...
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\dependencies\cmake_scripts" "..\dependencies\build_msvc8" "NMake Makefiles" RelWithDebInfo
@pushd ..\dependencies\build_msvc8\RelWithDebInfo
nmake
@popd


@echo Done
@pause