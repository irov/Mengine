#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CMAKE=cmake

if test -x "/usr/local/bin/cmake"; then
    CMAKE=/usr/local/bin/cmake
fi

if test -x "/Applications/CMake.app/Contents/bin/cmake"; then
    CMAKE=/Applications/CMake.app/Contents/bin/cmake
fi

CONFIGURATION="${1:-}"
DEPLOY_PATH="${2:-}"
BUILD_NUMBER="${3:-}"
BUILD_VERSION="${4:-}"
APPLICATION_OUTPUT_PATH="${5:-}"

if test -z "${CONFIGURATION}"; then
    echo "please setup CONFIGURATION"
    exit 1
fi

if test -z "${DEPLOY_PATH}"; then
    echo "please setup DEPLOY_PATH"
    exit 1
fi

if test -z "${BUILD_NUMBER}"; then
    echo "please setup BUILD_NUMBER"
    exit 1
fi

if test -z "${BUILD_VERSION}"; then
    echo "please setup BUILD_VERSION"
    exit 1
fi

if test ! -d "${DEPLOY_PATH}"; then
    echo "deploy path does not exist: ${DEPLOY_PATH}"
    exit 1
fi

DEPLOY_PATH="$(cd "${DEPLOY_PATH}" && pwd -P)"

if test -n "${APPLICATION_OUTPUT_PATH}"; then
    mkdir -p "${APPLICATION_OUTPUT_PATH}"
    APPLICATION_OUTPUT_PATH="$(cd "${APPLICATION_OUTPUT_PATH}" && pwd -P)"
fi

SOLUTION_NAME=solution_xcode_macos
SOLUTION_DIR="${SCRIPT_DIR}/../../solutions/${SOLUTION_NAME}/${CONFIGURATION}"
SOURCE_DIRECTORY="${SCRIPT_DIR}/../../cmake/Xcode_MacOS"
DEPENDENCIES_PROJECT=Depends_Xcode_MacOS

mkdir -p "${SOLUTION_DIR}"

"${CMAKE}" \
    -G Xcode \
    -S "${SOURCE_DIRECTORY}" \
    -B "${SOLUTION_DIR}" \
    -DCMAKE_BUILD_TYPE:STRING="${CONFIGURATION}" \
    -DCMAKE_CONFIGURATION_TYPES:STRING="${CONFIGURATION}" \
    -DMENGINE_DEPENDENCIES_PROJECT:STRING="${DEPENDENCIES_PROJECT}" \
    -DMENGINE_DEPLOY_PATH:PATH="${DEPLOY_PATH}" \
    -DMENGINE_BUILD_NUMBER:STRING="${BUILD_NUMBER}" \
    -DMENGINE_BUILD_VERSION:STRING="${BUILD_VERSION}" \
    -DMENGINE_APPLICATION_OUTPUT_PATH:PATH="${APPLICATION_OUTPUT_PATH}"

if test -f "${SOLUTION_DIR}/Podfile"; then
    (
        cd "${SOLUTION_DIR}"
        pod install --repo-update
    )
fi
