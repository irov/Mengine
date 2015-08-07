@echo off

set build_dir=%cd%
set cmd_vcvars="c:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
if exist %cmd_vcvars% goto vs_vars_found
set cmd_vcvars="c:\Program Files\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
:vs_vars_found
if not exist %cmd_vcvars% goto vs_not_found
set cmd_msbuild=msbuild

call %cmd_vcvars% x86
if errorlevel 1 goto error

@echo Starting dependencies build debug configuration...

@pushd ..
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\Depends_WIN32" "..\dependencies\build_msvc14" "NMake Makefiles" Debug
@popd

@pushd ..\..\dependencies\build_msvc14\Debug
nmake
@popd

@echo Done
goto exit

:vs_not_found
echo Visual Studio 2014 not found. Make sure it is installed to standard directory.
:error
if not "%1"=="batch" pause
exit /b 1
:exit
@pause