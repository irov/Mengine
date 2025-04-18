@echo off

setlocal
call :setESC

for %%O in (%*) do for /f "tokens=1,2 delims==" %%a in (%%O) do set "%%~a=%%~b"

echo **********************************************************************
echo SOLUTION_NAME: %SOLUTION_NAME%
echo SOURCE_DIRECTORY: %SOURCE_DIRECTORY%
echo CONFIGURATION: %CONFIGURATION%
echo GENERATOR: %GENERATOR%
echo **********************************************************************
echo ARCHITECTURE: %ARCHITECTURE%
echo PLATFORM_TOOLSET: %PLATFORM_TOOLSET%
echo BUILD: %BUILD%
echo VERBOSITY: %VERBOSITY%
echo **********************************************************************

if not defined SOLUTION_NAME (
    @echo invalid arguments, please setup SOLUTION_NAME
    exit 1
)

if not defined SOURCE_DIRECTORY (
    @echo invalid arguments, please setup SOURCE_DIRECTORY
    exit 1
)

if not defined CONFIGURATION (
    @echo invalid arguments, please setup CONFIGURATION
    exit 1
)

if not defined GENERATOR (
    @echo invalid arguments, please setup GENERATOR
    exit 1
)

if defined ARCHITECTURE (
    set "CMAKE_ARCHITECTURE=-A %ARCHITECTURE%"
) else (
    set "CMAKE_ARCHITECTURE="
)

if defined PLATFORM_TOOLSET (
    set "CMAKE_PLATFORM_TOOLSET=-T %PLATFORM_TOOLSET%"
) else (
    set "CMAKE_PLATFORM_TOOLSET="
)

if defined VERBOSITY (
    set "CMAKE_VERBOSITY=-- /verbosity:%VERBOSITY%"
) else (
    set "CMAKE_VERBOSITY="
)

set "BUILD_TEMP_DIR=%~dp0..\solutions\%SOLUTION_NAME%\%CONFIGURATION%"

@mkdir "%BUILD_TEMP_DIR%"

@pushd "%BUILD_TEMP_DIR%"
CMake -G "%GENERATOR%" %CMAKE_PLATFORM_TOOLSET% %CMAKE_ARCHITECTURE% -S "%SOURCE_DIRECTORY%" "-DCMAKE_CONFIGURATION_TYPES:STRING='%CONFIGURATION%'" "-DCMAKE_BUILD_TYPE:STRING='%CONFIGURATION%'"

if %errorlevel% NEQ 0 (
    @echo %ESC%[91m*****************************************%ESC%[0m
    @echo %ESC%[91m***************  Failure  ***************%ESC%[0m
    @echo %ESC%[91m*****************************************%ESC%[0m
    goto end
) else (
    @echo %ESC%[92m=========================================%ESC%[0m
    @echo %ESC%[92m=============  Successful  ==============%ESC%[0m
    @echo %ESC%[92m=========================================%ESC%[0m
)
@popd

if defined BUILD (
    if %BUILD% NEQ 0 (
        @pushd "%BUILD_TEMP_DIR%"
        CMake --build .\ --parallel 8 --config %CONFIGURATION% %CMAKE_VERBOSITY%

        if %errorlevel% NEQ 0 (
            @echo %ESC%[91m*****************************************%ESC%[0m
            @echo %ESC%[91m***************  Failure  ***************%ESC%[0m
            @echo %ESC%[91m*****************************************%ESC%[0m
            goto end
        ) else (
            @echo %ESC%[92m=========================================%ESC%[0m
            @echo %ESC%[92m=============  Successful  ==============%ESC%[0m
            @echo %ESC%[92m=========================================%ESC%[0m
        )
        @popd
    )
)

:end

exit /b %errorlevel%

:setESC
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
  set ESC=%%b
  exit /B 0
)