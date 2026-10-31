@echo off

setlocal EnableExtensions

set "CONFIGURATION=%~1"

if "%CONFIGURATION%" == "" (
    echo Usage: %~nx0 ^<Debug^|Release^> 1>&2
    exit /b 2
)

set "REPOSITORY_DIR=%~dp0..\.."
set "BUILD_DIR=%REPOSITORY_DIR%\solutions\dependencies_emscripten\%CONFIGURATION%"

call "%REPOSITORY_DIR%\dependencies\emsdk\emsdk_env.bat"
if errorlevel 1 exit /b %errorlevel%

cmake -G Ninja ^
    -S "%REPOSITORY_DIR%\cmake\Depends_EMSCRIPTEN" ^
    -B "%BUILD_DIR%" ^
    "-DCMAKE_TOOLCHAIN_FILE:FILEPATH=%EMSDK%\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake" ^
    "-DCMAKE_BUILD_TYPE:STRING=%CONFIGURATION%"
if errorlevel 1 exit /b %errorlevel%

cmake --build "%BUILD_DIR%" --parallel 8 --config %CONFIGURATION%
exit /b %errorlevel%
