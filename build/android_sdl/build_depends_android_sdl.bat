@echo off

if ["%~1"]==[""] (
  @echo invalid arguments, please select configuration
  goto end
)

set "CONFIGURATION=%1"

@echo Starting dependencies build %CONFIGURATION% configuration...

set ANDROID_SDK=%LOCALAPPDATA%\Android\sdk
set ANDROID_NDK=%ANDROID_SDK%\ndk-bundle
set ANDROID_SYSROOT=%ANDROID_NDK%\sysroot
set MAKE_PROGRAM=%ANDROID_SDK%\cmake\3.6.4111459\bin\ninja.exe
set TOOLCHAIN_FILE=%ANDROID_NDK%\build\cmake\android.toolchain.cmake
set CMAKE_EXE=%ANDROID_SDK%\cmake\3.6.4111459\bin\cmake.exe
set CMAKE_PATH="%CD%\..\..\cmake\Depends_Android_SDL"
set BUILD_TEMP_DIR="%CD%\..\..\solutions\dependencies_android_sdl"

@mkdir %BUILD_TEMP_DIR%\%CONFIGURATION%\armeabi-v7a
@pushd %BUILD_TEMP_DIR%\%CONFIGURATION%\armeabi-v7a

%CMAKE_EXE% -G "Android Gradle - Ninja" ^
    -DANDROID_PLATFORM=android-18 ^
    -DANDROID_ARM_NEON=TRUE ^
    -DANDROID_ABI=armeabi-v7a ^
    -DANDROID_STL=c++_shared ^
    -DANDROID_TOOLCHAIN=clang ^
    -DCMAKE_BUILD_TYPE=%CONFIGURATION% ^
    -DCMAKE_CONFIGURATION_TYPES:STRING=%CONFIGURATION% ^
    -DCMAKE_MAKE_PROGRAM=%MAKE_PROGRAM% ^
    -DCMAKE_TOOLCHAIN_FILE=%TOOLCHAIN_FILE% ^
    -DCMAKE_SYSROOT_COMPILE=%ANDROID_SYSROOT% ^
    %CMAKE_PATH%
    
%CMAKE_EXE% --build .\ --config %CONFIGURATION%

@popd

@mkdir %BUILD_TEMP_DIR%\%CONFIGURATION%\arm64-v8a
@pushd %BUILD_TEMP_DIR%\%CONFIGURATION%\arm64-v8a

%CMAKE_EXE% -G "Android Gradle - Ninja" ^
    -DANDROID_PLATFORM=android-18 ^
    -DANDROID_ARM_NEON=TRUE ^
    -DANDROID_ABI=arm64-v8a ^
    -DANDROID_STL=c++_shared ^
    -DANDROID_TOOLCHAIN=clang ^
    -DCMAKE_BUILD_TYPE=%CONFIGURATION% ^
    -DCMAKE_CONFIGURATION_TYPES:STRING=%CONFIGURATION% ^
    -DCMAKE_MAKE_PROGRAM=%MAKE_PROGRAM% ^
    -DCMAKE_TOOLCHAIN_FILE=%TOOLCHAIN_FILE% ^
    -DCMAKE_SYSROOT_COMPILE=%ANDROID_SYSROOT% ^
    %CMAKE_PATH%
    
%CMAKE_EXE% --build .\ --config %CONFIGURATION%
    
@popd

@echo Done

@pause