CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

OPTION(MENGINE_DOWNLOADS_SILENT "Mengine download silent" OFF)

PROJECT(Downloads)

SET(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
SET(MENGINE_DOWNLOADS_PATH "${PROJECT_DIR}/resources/.downloads" CACHE STRING "MENGINE_DOWNLOADS_PATH" FORCE)

include(./../../../../cmake/download_template.cmake)

DOWNLOAD_FILE(assets https://raw.githubusercontent.com/ggavryly/tmp_images/master/image_0.jpg image_0.jpg)
DOWNLOAD_FILE(assets https://raw.githubusercontent.com/ggavryly/tmp_images/master/image_1.jpg image_1.jpg)
DOWNLOAD_FILE(assets https://raw.githubusercontent.com/ggavryly/tmp_images/master/image_2.jpg image_2.jpg)
DOWNLOAD_FILE(assets https://raw.githubusercontent.com/ggavryly/tmp_images/master/image_3.jpg image_3.jpg)