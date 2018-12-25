@echo off

set BUILD_TYPE=Debug

@echo Starting dependencies build %BUILD_TYPE% configuration...

set ANDROID_SDK=%LOCALAPPDATA%\Android\sdk
set ANDROID_NDK=%ANDROID_SDK%\ndk-bundle
set ANDROID_SYSROOT=%ANDROID_NDK%\sysroot
set MAKE_PROGRAM=%ANDROID_SDK%\cmake\3.6.4111459\bin\ninja.exe
set TOOLCHAIN_FILE=%ANDROID_NDK%\build\cmake\android.toolchain.cmake
set CMAKE_EXE=%ANDROID_SDK%\cmake\3.6.4111459\bin\cmake.exe
set CMAKE_PATH="%CD%\..\..\CMake\Depends_Android_SDL"
set BUILD_TEMP_DIR="%CD%\..\..\build_temp\build_android_sdl_%BUILD_TYPE%"

@mkdir %BUILD_TEMP_DIR%
@pushd %BUILD_TEMP_DIR%

%CMAKE_EXE% -G "Android Gradle - Ninja" ^
    -DANDROID_PLATFORM=android-15 ^
    -DANDROID_ARM_NEON=TRUE ^
    -DANDROID_ABI=armeabi-v7a ^
    -DANDROID_STL=c++_shared ^
    -DANDROID_TOOLCHAIN=clang ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -DCMAKE_CONFIGURATION_TYPES:STRING=%BUILD_TYPE% ^
    -DCMAKE_MAKE_PROGRAM=%MAKE_PROGRAM% ^
    -DCMAKE_TOOLCHAIN_FILE=%TOOLCHAIN_FILE% ^
    -DCMAKE_SYSROOT_COMPILE=%ANDROID_SYSROOT% ^
    %CMAKE_PATH%
    
%CMAKE_EXE% --build .\ --config %BUILD_TYPE%
    
@popd

@echo Done

@pause