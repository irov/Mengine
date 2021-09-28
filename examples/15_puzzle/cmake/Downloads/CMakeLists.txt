CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

OPTION(MENGINE_DOWNLOADS_SILENT "Mengine download silent" OFF)

PROJECT(Downloads)

SET(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
SET(MENGINE_DOWNLOADS_PATH "${PROJECT_DIR}/resources/.downloads" CACHE STRING "MENGINE_DOWNLOADS_PATH" FORCE)

include(./../../../../cmake/download_template.cmake)

DOWNLOAD_FILE(assets http://www.pngmart.com/files/11/Trollface-PNG-Image.png image_0.png)
DOWNLOAD_FILE(assets http://pngimg.com/uploads/trollface/trollface_PNG23.png image_1.png)
DOWNLOAD_FILE(fonts https://github.com/google/fonts/raw/main/apache/roboto/static/Roboto-Black.ttf Roboto-Black.ttf)