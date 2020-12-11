@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

setlocal
call :setESC

set CONFIGURATION=%1

@echo Starting dependencies build %CONFIGURATION% configuration...

set ANDROID_NDK_VERSION=21.3.6528147
set ANDROID_CMAKE_VERSION=3.10.2.4988404

set ANDROID_PLATFORM=android-19
set ANDROID_SDK=%LOCALAPPDATA%\Android\sdk
set ANDROID_NDK=%ANDROID_SDK%\ndk\%ANDROID_NDK_VERSION%
set CMAKE_GENERATOR="Ninja"
set CMAKE_PATH=%ANDROID_SDK%\cmake\%ANDROID_CMAKE_VERSION%
set CMAKE_EXE=%CMAKE_PATH%\bin\cmake.exe
set CMAKE_MAKE_PROGRAM=%CMAKE_PATH%\bin\ninja.exe
set CMAKE_TOOLCHAIN_FILE=%ANDROID_NDK%\build\cmake\android.toolchain.cmake

set CMAKELIST_PATH=%CD%\..\..\cmake\Depends_Android_SDL
set BUILD_TEMP_DIR=%CD%\..\..\solutions\dependencies_android_sdl

@mkdir %BUILD_TEMP_DIR%\%CONFIGURATION%\x86
@pushd %BUILD_TEMP_DIR%\%CONFIGURATION%\x86

%CMAKE_EXE% -G %CMAKE_GENERATOR% ^
    -DANDROID_PLATFORM=%ANDROID_PLATFORM% ^
    -DANDROID_ARM_NEON=FALSE ^
    -DANDROID_ABI=x86 ^
    -DANDROID_NDK=%ANDROID_NDK% ^
    -DANDROID_STL=c++_shared ^
    -DANDROID_TOOLCHAIN=clang ^
    -DCMAKE_BUILD_TYPE=%CONFIGURATION% ^
    -DCMAKE_CONFIGURATION_TYPES:STRING=%CONFIGURATION% ^
    -DCMAKE_MAKE_PROGRAM=%CMAKE_MAKE_PROGRAM% ^
    -DCMAKE_TOOLCHAIN_FILE=%CMAKE_TOOLCHAIN_FILE% ^
    -S %CMAKELIST_PATH%
    
%CMAKE_EXE% --build .\ --config %CONFIGURATION% -- -j8

@popd

@mkdir %BUILD_TEMP_DIR%\%CONFIGURATION%\x86_64
@pushd %BUILD_TEMP_DIR%\%CONFIGURATION%\x86_64

%CMAKE_EXE% -G %CMAKE_GENERATOR% ^
    -DANDROID_PLATFORM=%ANDROID_PLATFORM% ^
    -DANDROID_ARM_NEON=FALSE ^
    -DANDROID_ABI=x86_64 ^
    -DANDROID_NDK=%ANDROID_NDK% ^
    -DANDROID_STL=c++_shared ^
    -DANDROID_TOOLCHAIN=clang ^
    -DCMAKE_BUILD_TYPE=%CONFIGURATION% ^
    -DCMAKE_CONFIGURATION_TYPES:STRING=%CONFIGURATION% ^
    -DCMAKE_MAKE_PROGRAM=%CMAKE_MAKE_PROGRAM% ^
    -DCMAKE_TOOLCHAIN_FILE=%CMAKE_TOOLCHAIN_FILE% ^
    -S %CMAKELIST_PATH%
    
%CMAKE_EXE% --build .\ --config %CONFIGURATION% -- -j8

@popd

@mkdir %BUILD_TEMP_DIR%\%CONFIGURATION%\armeabi-v7a
@pushd %BUILD_TEMP_DIR%\%CONFIGURATION%\armeabi-v7a

%CMAKE_EXE% -G %CMAKE_GENERATOR% ^
    -DANDROID_PLATFORM=%ANDROID_PLATFORM% ^
    -DANDROID_ARM_NEON=TRUE ^
    -DANDROID_ABI=armeabi-v7a ^
    -DANDROID_NDK=%ANDROID_NDK% ^
    -DANDROID_STL=c++_shared ^
    -DANDROID_TOOLCHAIN=clang ^
    -DCMAKE_BUILD_TYPE=%CONFIGURATION% ^
    -DCMAKE_CONFIGURATION_TYPES:STRING=%CONFIGURATION% ^
    -DCMAKE_MAKE_PROGRAM=%CMAKE_MAKE_PROGRAM% ^
    -DCMAKE_TOOLCHAIN_FILE=%CMAKE_TOOLCHAIN_FILE% ^
    -S %CMAKELIST_PATH%
    
%CMAKE_EXE% --build .\ --config %CONFIGURATION% -- -j8

@popd

@mkdir %BUILD_TEMP_DIR%\%CONFIGURATION%\arm64-v8a
@pushd %BUILD_TEMP_DIR%\%CONFIGURATION%\arm64-v8a

%CMAKE_EXE% -G %CMAKE_GENERATOR% ^
    -DANDROID_PLATFORM=%ANDROID_PLATFORM% ^
    -DANDROID_ARM_NEON=TRUE ^
    -DANDROID_ABI=arm64-v8a ^
    -DANDROID_NDK=%ANDROID_NDK% ^
    -DANDROID_STL=c++_shared ^
    -DANDROID_TOOLCHAIN=clang ^
    -DCMAKE_BUILD_TYPE=%CONFIGURATION% ^
    -DCMAKE_CONFIGURATION_TYPES:STRING=%CONFIGURATION% ^
    -DCMAKE_MAKE_PROGRAM=%CMAKE_MAKE_PROGRAM% ^
    -DCMAKE_TOOLCHAIN_FILE=%CMAKE_TOOLCHAIN_FILE% ^
    -S %CMAKELIST_PATH%
    
%CMAKE_EXE% --build .\ --config %CONFIGURATION% -- -j8
    
@popd

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
  exit /B 0
)