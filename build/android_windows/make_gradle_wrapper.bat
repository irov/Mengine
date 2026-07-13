@echo off

setlocal EnableExtensions

for %%I in ("%~dp0..\..") do set "MENGINE_DIRECTORY=%%~fI"

set "GRADLE_PROJECT_DIRECTORY=%MENGINE_DIRECTORY%\gradle"
set "GRADLE_VERSION_FILE=%GRADLE_PROJECT_DIRECTORY%\gradle-wrapper-version.properties"

if not exist "%GRADLE_VERSION_FILE%" (
    echo Gradle wrapper version file not found: %GRADLE_VERSION_FILE%
    exit /b 1
)

for /f "usebackq eol=# tokens=1,* delims==" %%A in ("%GRADLE_VERSION_FILE%") do (
    if /I "%%A"=="GRADLE_VERSION" set "GRADLE_VERSION=%%B"
    if /I "%%A"=="GRADLE_DISTRIBUTION_SHA256" set "GRADLE_DISTRIBUTION_SHA256=%%B"
)

if not defined GRADLE_VERSION (
    echo Invalid Gradle wrapper version file: %GRADLE_VERSION_FILE%
    exit /b 1
)

if not defined GRADLE_DISTRIBUTION_SHA256 (
    echo Invalid Gradle wrapper version file: %GRADLE_VERSION_FILE%
    exit /b 1
)

where gradle >nul 2>nul

if errorlevel 1 (
    echo System Gradle not found. Install Gradle 9.4.1 or newer and add it to PATH.
    exit /b 1
)

call gradle --version

if errorlevel 1 (
    exit /b 1
)

call gradle ^
    --project-dir "%GRADLE_PROJECT_DIRECTORY%" ^
    -PANDROID_APP_MAIN_PROJECT=ci ^
    wrapper ^
    --gradle-version "%GRADLE_VERSION%" ^
    --distribution-type bin ^
    --gradle-distribution-sha256-sum "%GRADLE_DISTRIBUTION_SHA256%"

if errorlevel 1 (
    echo Gradle wrapper generation failed. Android Gradle Plugin 9.2.1 requires Gradle 9.4.1 or newer.
    exit /b 1
)

set "GRADLE_WRAPPER_DIRECTORY=%GRADLE_PROJECT_DIRECTORY%\gradle\wrapper"

if not exist "%GRADLE_PROJECT_DIRECTORY%\gradlew" (
    echo Gradle wrapper file not generated: %GRADLE_PROJECT_DIRECTORY%\gradlew
    exit /b 1
)

if not exist "%GRADLE_PROJECT_DIRECTORY%\gradlew.bat" (
    echo Gradle wrapper file not generated: %GRADLE_PROJECT_DIRECTORY%\gradlew.bat
    exit /b 1
)

if not exist "%GRADLE_WRAPPER_DIRECTORY%\gradle-wrapper.jar" (
    echo Gradle wrapper file not generated: %GRADLE_WRAPPER_DIRECTORY%\gradle-wrapper.jar
    exit /b 1
)

if not exist "%GRADLE_WRAPPER_DIRECTORY%\gradle-wrapper.properties" (
    echo Gradle wrapper file not generated: %GRADLE_WRAPPER_DIRECTORY%\gradle-wrapper.properties
    exit /b 1
)

findstr /C:"gradle-%GRADLE_VERSION%-bin.zip" "%GRADLE_WRAPPER_DIRECTORY%\gradle-wrapper.properties" >nul

if errorlevel 1 (
    echo Generated Gradle wrapper has an unexpected distribution version.
    exit /b 1
)

findstr /C:"distributionSha256Sum=%GRADLE_DISTRIBUTION_SHA256%" "%GRADLE_WRAPPER_DIRECTORY%\gradle-wrapper.properties" >nul

if errorlevel 1 (
    echo Generated Gradle wrapper has an unexpected distribution checksum.
    exit /b 1
)

echo Gradle wrapper %GRADLE_VERSION% generated successfully.

exit /b 0
