@echo off

setlocal
call :setESC

for %%O in (%*) do for /f "tokens=1,2 delims==" %%a in (%%O) do set "%%~a=%%~b"

echo **********************************************************************
echo SOLUTION_NAME: %SOLUTION_NAME%
echo CONFIGURATION: %CONFIGURATION%
echo GENERATOR: %GENERATOR%
echo PLATFORM_TOOLSET: %PLATFORM_TOOLSET%
echo DEPLOY_PATH: %DEPLOY_PATH%
echo EXTERNAL_PDB_PATH: %EXTERNAL_PDB_PATH%
echo ARCHITECTURE: %ARCHITECTURE%
echo SOURCE_DIRECTORY: %SOURCE_DIRECTORY%
echo VERBOSITY: %VERBOSITY%
echo BUILD_VERSION: %BUILD_VERSION%
echo **********************************************************************

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

if defined DEPLOY_PATH (
    set "MENGINE_DEPLOY_PATH=-DMENGINE_DEPLOY_PATH:STRING=%DEPLOY_PATH%"
) else (
    set "MENGINE_DEPLOY_PATH="
)

if defined VERBOSITY (
    set "CMAKE_VERBOSITY=-- /verbosity:%VERBOSITY%"
) else (
    set "CMAKE_VERBOSITY"
)

if defined EXTERNAL_PDB_PATH (
    set "MENGINE_EXTERNAL_PDB=-DMENGINE_EXTERNAL_PDB:BOOL=ON"
    set "MENGINE_EXTERNAL_PDB_PATH=-DMENGINE_EXTERNAL_PDB_PATH:STRING=%EXTERNAL_PDB_PATH%"
) else (
    set "MENGINE_EXTERNAL_PDB=-DMENGINE_EXTERNAL_PDB:BOOL=OFF"
    set "MENGINE_EXTERNAL_PDB_PATH="
)

if defined BUILD_VERSION (
    set "MENGINE_BUILD_VERSION=-DMENGINE_BUILD_VERSION:STRING=%BUILD_VERSION%"
) else (
    set "MENGINE_BUILD_VERSION=-DMENGINE_BUILD_VERSION:STRING=Versionless"
)

if defined BUILD_PUBLISH (
    set "MENGINE_BUILD_MENGINE_BUILD_PUBLISH=-DMENGINE_BUILD_MENGINE_BUILD_PUBLISH:BOOL=%BUILD_PUBLISH%"
) else (
    set "MENGINE_BUILD_MENGINE_BUILD_PUBLISH=-DMENGINE_BUILD_MENGINE_BUILD_PUBLISH:BOOL=OFF"
)

set "BUILD_TEMP_DIR=..\solutions\%SOLUTION_NAME%\%CONFIGURATION%"

@mkdir "%BUILD_TEMP_DIR%"
@pushd "%BUILD_TEMP_DIR%"

CMake -G "%GENERATOR%" %CMAKE_PLATFORM_TOOLSET% %CMAKE_ARCHITECTURE% -S "%SOURCE_DIRECTORY%" "-DCMAKE_BUILD_TYPE:STRING=%CONFIGURATION%" %MENGINE_EXTERNAL_PDB% %MENGINE_EXTERNAL_PDB_PATH% %MENGINE_BUILD_VERSION% %MENGINE_BUILD_MENGINE_BUILD_PUBLISH%

@popd

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

@pushd "%BUILD_TEMP_DIR%"

CMake --build .\ -j 8 --config %CONFIGURATION% %CMAKE_VERBOSITY%

@popd

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

:end

exit /b %errorlevel%

:setESC
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
  set ESC=%%b
  exit /B 0
)