@echo off

setlocal
call :setESC

for %%O in (%*) do for /f "tokens=1,2 delims==" %%a in (%%O) do set "%%~a=%%~b"

echo **********************************************************************
echo SOLUTION_NAME: %SOLUTION_NAME%
echo DEPENDENCIES_PROJECT: %DEPENDENCIES_PROJECT%
echo SOURCE_DIRECTORY: %SOURCE_DIRECTORY%
echo DEPENDENCIES_DIRECTORY: %DEPENDENCIES_DIRECTORY%
echo CONFIGURATION: %CONFIGURATION%
echo GENERATOR: %GENERATOR%
echo **********************************************************************
echo ARCHITECTURE: %ARCHITECTURE%
echo PLATFORM_TOOLSET: %PLATFORM_TOOLSET%
echo **********************************************************************
echo DEPLOY_PATH: %DEPLOY_PATH%
echo EXTERNAL_PDB_PATH: %EXTERNAL_PDB_PATH%
echo BUILD_NUMBER: %BUILD_NUMBER%
echo BUILD_VERSION: %BUILD_VERSION%
echo BUILD_PUBLISH: %BUILD_PUBLISH%
echo **********************************************************************
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

if not defined DEPENDENCIES_PROJECT (
    @echo invalid arguments, please setup DEPENDENCIES_PROJECT
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

if not defined SOLUTION_DIR (
    if defined SOLUTION_BASE_DIR (
        set "SOLUTION_DIR=%SOLUTION_BASE_DIR%\%SOLUTION_NAME%\%CONFIGURATION%"
    ) else (
        set "SOLUTION_DIR=%~dp0..\solutions\%SOLUTION_NAME%\%CONFIGURATION%"
    )
)

if defined PLATFORM_TOOLSET (
    set "CMAKE_PLATFORM_TOOLSET=-T %PLATFORM_TOOLSET%"
) else (
    set "CMAKE_PLATFORM_TOOLSET="
)

if defined DEPLOY_PATH (
    set "MENGINE_DEPLOY_PATH=-DMENGINE_DEPLOY_PATH:STRING=%DEPLOY_PATH%"
) else (
    set "MENGINE_DEPLOY_PATH="
)

if defined EXTERNAL_PDB_PATH (
    set "MENGINE_EXTERNAL_PDB=-DMENGINE_EXTERNAL_PDB:BOOL=ON"
    set "MENGINE_EXTERNAL_PDB_PATH=-DMENGINE_EXTERNAL_PDB_PATH:STRING=%EXTERNAL_PDB_PATH%"
) else (
    set "MENGINE_EXTERNAL_PDB=-DMENGINE_EXTERNAL_PDB:BOOL=OFF"
    set "MENGINE_EXTERNAL_PDB_PATH="
)

if defined BUILD_NUMBER (
    set "MENGINE_BUILD_NUMBER=-DMENGINE_BUILD_NUMBER:STRING=%BUILD_NUMBER%"
) else (
    set "MENGINE_BUILD_NUMBER=-DMENGINE_BUILD_NUMBER:STRING=0"
)

if defined BUILD_VERSION (
    set "MENGINE_BUILD_VERSION=-DMENGINE_BUILD_VERSION:STRING=%BUILD_VERSION%"
) else (
    set "MENGINE_BUILD_VERSION=-DMENGINE_BUILD_VERSION:STRING=0.0.0"
)

if defined BUILD_PUBLISH (
    set "MENGINE_BUILD_MENGINE_BUILD_PUBLISH=-DMENGINE_BUILD_MENGINE_BUILD_PUBLISH:BOOL=%BUILD_PUBLISH%"
) else (
    set "MENGINE_BUILD_MENGINE_BUILD_PUBLISH=-DMENGINE_BUILD_MENGINE_BUILD_PUBLISH:BOOL=OFF"
)

if defined VERBOSITY (
    set "CMAKE_VERBOSITY=-- /verbosity:%VERBOSITY%"
) else (
    set "CMAKE_VERBOSITY="
)

@mkdir "%SOLUTION_DIR%"

@pushd "%SOLUTION_DIR%"
CMake -G "%GENERATOR%" %CMAKE_PLATFORM_TOOLSET% %CMAKE_ARCHITECTURE% -S "%SOURCE_DIRECTORY%" "-DMENGINE_DEPENDENCIES_PROJECT:STRING='%DEPENDENCIES_PROJECT%'" "-DCMAKE_CONFIGURATION_TYPES:STRING='%CONFIGURATION%'" "-DCMAKE_BUILD_TYPE:STRING=%CONFIGURATION%" %MENGINE_DEPLOY_PATH% %MENGINE_EXTERNAL_PDB% %MENGINE_EXTERNAL_PDB_PATH% %MENGINE_BUILD_NUMBER% %MENGINE_BUILD_VERSION% %MENGINE_BUILD_MENGINE_BUILD_PUBLISH%

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
        @pushd "%SOLUTION_DIR%"
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