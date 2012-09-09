@echo off

set build_dir=%cd%
set cmd_vcvars="c:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
if exist %cmd_vcvars% goto vs_vars_found
set cmd_vcvars="c:\Program Files\Microsoft Visual Studio 10.0\VC\vcvarsall.bat"
:vs_vars_found
if not exist %cmd_vcvars% goto vs_not_found
set cmd_msbuild=msbuild

call %cmd_vcvars% x86
if errorlevel 1 goto error

@echo Starting dependencies build debug configuration...
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\Depends_WIN32" "..\dependencies\build_msvc10" "NMake Makefiles" Debug
@pushd ..\dependencies\build_msvc10\Debug
nmake
@popd

@echo Starting dependencies build release with debug info configuration...
@echo @call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\Depends_WIN32" "..\dependencies\build_msvc10" "NMake Makefiles" RelWithDebInfo
@echo @pushd ..\dependencies\build_msvc10\RelWithDebInfo
@echo nmake
@echo @popd

@echo Starting dependencies build release configuration...
@echo @call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\dependencies" "..\dependencies\build_msvc10" "NMake Makefiles" Release
@echo @pushd ..\dependencies\build_msvc10\Release
@echo nmake
@echo @popd

@echo Done
goto exit

:vs_not_found
echo Visual Studio 2010 not found. Make sure it is installed to standard directory.
:error
if not "%1"=="batch" pause
exit /b 1
:exit
@pause