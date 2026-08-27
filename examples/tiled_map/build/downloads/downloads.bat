@echo off

setlocal

set "SOLUTION_DIR=%~dp0..\..\solutions\downloads"
set "CMAKE_SOURCE_DIR=%~dp0..\..\cmake\Downloads"

if not exist "%SOLUTION_DIR%" mkdir "%SOLUTION_DIR%"

cmake -S "%CMAKE_SOURCE_DIR%" -B "%SOLUTION_DIR%" -DMENGINE_DOWNLOADS_SILENT=ON
if errorlevel 1 exit /b 1

cmake --build "%SOLUTION_DIR%" -- /verbosity:minimal
if errorlevel 1 exit /b 1

endlocal
