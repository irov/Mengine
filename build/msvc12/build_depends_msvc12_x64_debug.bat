@echo off

set build_dir=%cd%
set cmd_vcvars="c:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
if exist %cmd_vcvars% goto vs_vars_found
set cmd_vcvars="c:\Program Files\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
:vs_vars_found
if not exist %cmd_vcvars% goto vs_not_found
set cmd_msbuild=msbuild

call %cmd_vcvars% x64
if errorlevel 1 goto error

@echo Starting dependencies build debug configuration...

@pushd ..
@call cmake_configure "%CD%\..\dependencies\cmake\bin\cmake.exe" "%CD%\..\CMake\Depends_WIN64" "..\dependencies\build_msvc12_x64_debug" "Visual Studio 12 2013 Win64" "" "-DCMAKE_CONFIGURATION_TYPES:STRING='Debug'" "-DCMAKE_BUILD_TYPE:STRING='Debug'"
@popd

@pushd ..\..\dependencies\build_msvc12_x64_debug
..\..\dependencies\cmake\bin\cmake.exe --build .\ --config Debug
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