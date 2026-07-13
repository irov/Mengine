#!/bin/bash

set -e

SCRIPT_DIRECTORY="$(cd "$(dirname "$0")" && pwd)"
MENGINE_DIRECTORY="$(cd "$SCRIPT_DIRECTORY/../.." && pwd)"
GRADLE_PROJECT_DIRECTORY="$MENGINE_DIRECTORY/gradle"
GRADLE_VERSION_FILE="$GRADLE_PROJECT_DIRECTORY/gradle-wrapper-version.properties"

if [ ! -f "$GRADLE_VERSION_FILE" ]; then
    echo "Gradle wrapper version file not found: $GRADLE_VERSION_FILE"
    exit 1
fi

while IFS='=' read -r KEY VALUE; do
    case "$KEY" in
        GRADLE_VERSION)
            GRADLE_VERSION="$VALUE"
            ;;
        GRADLE_DISTRIBUTION_SHA256)
            GRADLE_DISTRIBUTION_SHA256="$VALUE"
            ;;
    esac
done < "$GRADLE_VERSION_FILE"

if [ -z "$GRADLE_VERSION" ] || [ -z "$GRADLE_DISTRIBUTION_SHA256" ]; then
    echo "Invalid Gradle wrapper version file: $GRADLE_VERSION_FILE"
    exit 1
fi

if ! command -v gradle >/dev/null 2>&1; then
    echo "System Gradle not found. Install Gradle 9.4.1 or newer and add it to PATH."
    exit 1
fi

gradle --version

if ! gradle \
    --project-dir "$GRADLE_PROJECT_DIRECTORY" \
    -PANDROID_APP_MAIN_PROJECT=ci \
    wrapper \
    --gradle-version "$GRADLE_VERSION" \
    --distribution-type bin \
    --gradle-distribution-sha256-sum "$GRADLE_DISTRIBUTION_SHA256"; then
    echo "Gradle wrapper generation failed. Android Gradle Plugin 9.2.1 requires Gradle 9.4.1 or newer."
    exit 1
fi

GRADLE_WRAPPER_DIRECTORY="$GRADLE_PROJECT_DIRECTORY/gradle/wrapper"

for WRAPPER_FILE in \
    "$GRADLE_PROJECT_DIRECTORY/gradlew" \
    "$GRADLE_PROJECT_DIRECTORY/gradlew.bat" \
    "$GRADLE_WRAPPER_DIRECTORY/gradle-wrapper.jar" \
    "$GRADLE_WRAPPER_DIRECTORY/gradle-wrapper.properties"; do
    if [ ! -f "$WRAPPER_FILE" ]; then
        echo "Gradle wrapper file not generated: $WRAPPER_FILE"
        exit 1
    fi
done

if ! grep -Fq "gradle-$GRADLE_VERSION-bin.zip" "$GRADLE_WRAPPER_DIRECTORY/gradle-wrapper.properties"; then
    echo "Generated Gradle wrapper has an unexpected distribution version."
    exit 1
fi

if ! grep -Fq "distributionSha256Sum=$GRADLE_DISTRIBUTION_SHA256" "$GRADLE_WRAPPER_DIRECTORY/gradle-wrapper.properties"; then
    echo "Generated Gradle wrapper has an unexpected distribution checksum."
    exit 1
fi

chmod +x "$GRADLE_PROJECT_DIRECTORY/gradlew"

echo "Gradle wrapper $GRADLE_VERSION generated successfully."
