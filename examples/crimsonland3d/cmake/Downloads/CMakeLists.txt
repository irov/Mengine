CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

OPTION(MENGINE_DOWNLOADS_SILENT "downloads silent" OFF)

PROJECT(Downloads)

SET(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
SET(MENGINE_DOWNLOADS_PATH "${PROJECT_DIR}/resources/.downloads" CACHE STRING "MENGINE_DOWNLOADS_PATH" FORCE)

include(./../../../../cmake/download_template.cmake)

DOWNLOAD_FILE(fonts https://github.com/googlefonts/roboto-2/raw/main/src/hinted/Roboto-Bold.ttf Roboto-Bold.ttf)
DOWNLOAD_FILE(fonts https://github.com/googlefonts/roboto-2/raw/main/LICENSE Roboto_license.txt)

# Monster meshes + shared atlas are fetched from a public Google Drive folder
# instead of being committed to the repository.
DOWNLOAD_GDRIVE_FOLDER(models 1sOXLt5U3ofaujPlQRL11s4ub2UsqN8V8 ${MENGINE_DOWNLOADS_PATH}/models)
