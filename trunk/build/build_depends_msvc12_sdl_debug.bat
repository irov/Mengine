@echo off

set build_dir=%cd%
set cmd_vcvars="c:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
if exist %cmd_vcvars% goto vs_vars_found
set cmd_vcvars="c:\Program Files\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
:vs_vars_found
if not exist %cmd_vcvars% goto vs_not_found
set cmd_msbuild=msbuild

call %cmd_vcvars% x86
if errorlevel 1 goto error

@echo Starting dependencies build debug configuration...
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\Depends_SDL" "..\dependencies\build_sdl" "NMake Makefiles" Debug
@pushd ..\dependencies\build_sdl\Debug
nmake
@popd

@echo Done
goto exit

:vs_not_found
echo Visual Studio 2012 not found. Make sure it is installed to standard directory.
:error
if not "%1"=="batch" pause
exit /b 1
:exit
@pause