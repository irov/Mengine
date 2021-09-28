CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

OPTION(MENGINE_DOWNLOADS_SILENT "downloads silent" OFF)

PROJECT(Downloads)

SET(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
SET(MENGINE_DOWNLOADS_PATH "${PROJECT_DIR}/resources/.downloads" CACHE STRING "MENGINE_DOWNLOADS_PATH" FORCE)

include(./../../../../cmake/download_template.cmake)

DOWNLOAD_FILE(assets https://pixel-grass.preventslimbaugh.fun/img/248047.png grass.png)
DOWNLOAD_FILE(assets https://art.pixilart.com/86f56a77d8e1c5f.png stone.png)
DOWNLOAD_FILE(fonts https://github.com/google/fonts/blob/master/apache/roboto/static/Roboto-Black.ttf?raw=true Roboto-Black.ttf)