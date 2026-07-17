@echo off

set "CONFIGURATION=%~1"
if "%CONFIGURATION%"=="" set "CONFIGURATION=Debug"

@call %~dp0make_solution.bat "CONFIGURATION=%CONFIGURATION%"
if errorlevel 1 exit /b %errorlevel%

set "SOLUTION_DIR=%~dp0..\..\solutions\solution_msvc18_dx11_x64\%CONFIGURATION%"
cmake --build "%SOLUTION_DIR%" --config "%CONFIGURATION%" --target Arena3DServer Arena3DBotClient Arena3DNetProxy Win32Application
exit /b %errorlevel%
