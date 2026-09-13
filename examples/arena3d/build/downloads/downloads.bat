@echo off
setlocal

set "PROJECT_DIR=%~dp0..\.."

cmake -S "%PROJECT_DIR%\cmake\downloads" -B "%PROJECT_DIR%\solutions\downloads"
if errorlevel 1 exit /b 1

cmake --build "%PROJECT_DIR%\solutions\downloads"
if errorlevel 1 exit /b 1
