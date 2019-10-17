CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

OPTION(MENGINE_DOWNLOADS_SILENT "Mengine download silent" OFF)

PROJECT(Downloads)

SET(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
SET(MENGINE_DOWNLOADS_PATH "${PROJECT_DIR}/resources/.downloads" CACHE STRING "MENGINE_DOWNLOADS_PATH" FORCE)

include(./../../../../cmake/download_template.cmake)

DOWNLOAD_FILE(assets https://raw.githubusercontent.com/LazoCoder/Tic-Tac-Toe/master/TicTacToe/Assets/background.png background.png)
DOWNLOAD_FILE(assets https://raw.githubusercontent.com/LazoCoder/Tic-Tac-Toe/master/TicTacToe/Assets/o.png o.png)
DOWNLOAD_FILE(assets https://raw.githubusercontent.com/LazoCoder/Tic-Tac-Toe/master/TicTacToe/Assets/x.png x.png)