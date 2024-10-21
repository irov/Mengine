set "CONFIGURATION=%1"
set "SOLUTION_NAME=dependencies_emscripten"
set "SOURCE_DIRECTORY=%~dp0..\..\cmake\Depends_EMSCRIPTEN"

@call "%~dp0..\..\dependencies\emsdk\emsdk_env.bat"

set "BUILD_TEMP_DIR=%~dp0..\..\solutions\%SOLUTION_NAME%\%CONFIGURATION%"

@mkdir "%BUILD_TEMP_DIR%"

@pushd "%BUILD_TEMP_DIR%"

@call emcmake cmake -S "%SOURCE_DIRECTORY%" -DCMAKE_CONFIGURATION_TYPES:STRING=%CONFIGURATION% -DCMAKE_BUILD_TYPE:STRING=%CONFIGURATION%

@popd

@exit /b %errorlevel%