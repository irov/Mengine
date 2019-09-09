@echo off

set build_dir=%cd%
set cmd_vcvars="C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
if exist %cmd_vcvars% goto vs_vars_found
set cmd_vcvars="C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat"
if not exist %cmd_vcvars% goto vs_not_found
:vs_vars_found

call %cmd_vcvars% x86
if errorlevel 1 goto error

goto successful

:vs_not_found
echo Visual Studio 2019 not found. Make sure it is installed to standard directory.
:error
exit /b 1

:successful