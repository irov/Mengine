@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

setlocal
call :setESC

set CONFIGURATION=%1

@echo Starting android dependencies build %CONFIGURATION% configuration...

set ANDROID_NDK_VERSION=26.3.11579264
set ANDROID_CMAKE_VERSION=3.22.1

set ANDROID_PLATFORM_VERSION=21
set ANDROID_PLATFORM=android-%ANDROID_PLATFORM_VERSION%
set ANDROID_SDK=%LOCALAPPDATA%\Android\sdk
set ANDROID_NDK=%ANDROID_SDK%\ndk\%ANDROID_NDK_VERSION%
set CMAKE_GENERATOR="Ninja"
set CMAKE_PATH=%ANDROID_SDK%\cmake\%ANDROID_CMAKE_VERSION%
set CMAKE_EXE=%CMAKE_PATH%\bin\cmake.exe
set CMAKE_MAKE_PROGRAM=%CMAKE_PATH%\bin\ninja.exe
set CMAKE_TOOLCHAIN_FILE=%ANDROID_NDK%\build\cmake\android.toolchain.cmake

set CMAKELIST_PATH=%~dp0..\..\cmake\Depends_Android_SDL
set BUILD_TEMP_DIR=%~dp0..\..\solutions\dependencies_android_sdl

for %%A in (x86 x86_64 armeabi-v7a arm64-v8a) do (
    @mkdir %BUILD_TEMP_DIR%\%CONFIGURATION%\%%A
    @pushd %BUILD_TEMP_DIR%\%CONFIGURATION%\%%A

    @call %CMAKE_EXE% -G %CMAKE_GENERATOR% ^
        -DANDROID_PLATFORM=%ANDROID_PLATFORM% ^
        -DANDROID_ARM_NEON=TRUE ^
        -DANDROID_ABI=%%A ^
        -DANDROID_NDK=%ANDROID_NDK% ^
        -DANDROID_STL=c++_shared ^
        -DANDROID_TOOLCHAIN=clang ^
        -DCMAKE_SYSTEM_NAME=Android ^
        -DCMAKE_SYSTEM_VERSION=%ANDROID_PLATFORM_VERSION% ^
        -DCMAKE_BUILD_TYPE=%CONFIGURATION% ^
        -DCMAKE_CONFIGURATION_TYPES:STRING=%CONFIGURATION% ^
        -DCMAKE_MAKE_PROGRAM=%CMAKE_MAKE_PROGRAM% ^
        -DCMAKE_TOOLCHAIN_FILE=%CMAKE_TOOLCHAIN_FILE% ^
        -S %CMAKELIST_PATH%
        
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
    
    @call %CMAKE_EXE% --build .\ --config %CONFIGURATION% -- -j8
    
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

if %errorlevel% NEQ 0 (
    @echo %ESC%[91m*****************************************%ESC%[0m
    @echo %ESC%[91m***************  Failure  ***************%ESC%[0m
    @echo %ESC%[91m*****************************************%ESC%[0m
    goto :end
) else (
    @echo %ESC%[92m=========================================%ESC%[0m
    @echo %ESC%[92m=============  Successful  ==============%ESC%[0m
    @echo %ESC%[92m=========================================%ESC%[0m
)

:end
@echo Done

@pause

:setESC
for /F "tokens=1,2 delims=#" %%a in ('"prompt #$H#$E# & echo on & for %%b in (1) do rem"') do (
  set ESC=%%b
)

exit /B 0