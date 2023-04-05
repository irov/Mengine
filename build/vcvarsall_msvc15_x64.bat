@echo off

set cmd_vcvars="%ProgramFiles(x86)%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"
if exist %cmd_vcvars% goto vs_vars_found
set cmd_vcvars="%ProgramFiles%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat"
if exist %cmd_vcvars% goto vs_vars_found

goto vs_not_found

:vs_vars_found

call %cmd_vcvars% x64
if %errorlevel% NEQ 0 goto error

goto successful

:vs_not_found
echo Visual Studio 2017 not found. Make sure it is installed to standard directory.
:error
exit /b 1

:successful
exit /b 0