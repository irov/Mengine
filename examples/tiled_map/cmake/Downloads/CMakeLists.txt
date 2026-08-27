CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

OPTION(MENGINE_DOWNLOADS_SILENT "downloads silent" OFF)

PROJECT(Downloads)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

SET(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
SET(MENGINE_DOWNLOADS_PATH "${PROJECT_DIR}/resources/.downloads" CACHE STRING "MENGINE_DOWNLOADS_PATH" FORCE)

include(./../../../../cmake/download_template.cmake)

set(TILED_EXAMPLES_SOURCE_DIR "${MENGINE_DOWNLOADS_PATH}/tiled")

ExternalProject_Add(tiled_examples_download PREFIX tiled_examples
    SOURCE_DIR "${TILED_EXAMPLES_SOURCE_DIR}"

    URL "https://github.com/mapeditor/tiled/archive/refs/heads/master.zip"
    DOWNLOAD_NO_PROGRESS ${MENGINE_DOWNLOAD_NO_PROGRESS}

    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    UPDATE_COMMAND ""
    INSTALL_COMMAND ""
)
