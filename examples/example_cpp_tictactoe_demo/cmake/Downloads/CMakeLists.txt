CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

OPTION(MENGINE_DOWNLOADS_SILENT "Mengine download silent" OFF)

PROJECT(Downloads)

SET(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
SET(THIRDPARTY_DIR ${PROJECT_DIR}/dependencies)

include(./../../../../CMake/Downloads/download_template.cmake)

DOWNLOAD_FILE(assets https://raw.githubusercontent.com/LazoCoder/Tic-Tac-Toe/master/TicTacToe/Assets/background.png background.png)
DOWNLOAD_FILE(assets https://raw.githubusercontent.com/LazoCoder/Tic-Tac-Toe/master/TicTacToe/Assets/o.png o.png)
DOWNLOAD_FILE(assets https://raw.githubusercontent.com/LazoCoder/Tic-Tac-Toe/master/TicTacToe/Assets/x.png x.png)