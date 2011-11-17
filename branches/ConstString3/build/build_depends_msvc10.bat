@echo off

@echo Starting dependencies build debug configuration...
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\dependencies" "..\dependencies\build_msvc10" "NMake Makefiles" Debug
@pushd ..\dependencies\build_msvc10\Debug
nmake
@popd

@echo Starting dependencies build release configuration...
@echo @call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\dependencies" "..\dependencies\build_msvc10" "NMake Makefiles" Release
@echo @pushd ..\dependencies\build_msvc10\Release
@echo nmake
@echo @popd

@echo Starting dependencies build release with debug info configuration...
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\dependencies" "..\dependencies\build_msvc10" "NMake Makefiles" RelWithDebInfo
@pushd ..\dependencies\build_msvc10\RelWithDebInfo
nmake
@popd

@echo Done
@pause