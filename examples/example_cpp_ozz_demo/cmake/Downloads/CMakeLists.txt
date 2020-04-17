CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

OPTION(MENGINE_DOWNLOADS_SILENT "downloads silent" OFF)

PROJECT(Downloads)

SET(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
SET(MENGINE_DOWNLOADS_PATH "${PROJECT_DIR}/resources/.downloads" CACHE STRING "MENGINE_DOWNLOADS_PATH" FORCE)

include(./../../../../cmake/download_template.cmake)

DOWNLOAD_FILE(fonts https://github.com/google/fonts/blob/master/apache/roboto/Roboto-Black.ttf?raw=true Roboto-Black.ttf)
