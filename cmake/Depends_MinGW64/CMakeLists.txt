CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

SET(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/../mingw64_clang_toolchains.cmake")

PROJECT(Depends_MinGW64)

SET(MENGINE_REPOSITORY ${CMAKE_CURRENT_LIST_DIR}/../..)

INCLUDE(${MENGINE_REPOSITORY}/cmake/build_options_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/base_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/macro_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/mingw64_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/dependencies_template.cmake)

SET_MENGINE_ENVIRONMENT(WINDOWS DIRECTX9 WIN32 MINGW)

SET_MENGINE_DEPENDENCIES_OUTPUT_DIRECTORY()

SET(MENGINE_DEPENDENCE_OZZ OFF CACHE BOOL "MENGINE_DEPENDENCE_OZZ" FORCE)
SET(MENGINE_DEPENDENCE_SDL2 OFF CACHE BOOL "MENGINE_DEPENDENCE_SDL2" FORCE)

ADD_SUBDIRECTORY(${CMAKE_CURRENT_LIST_DIR}/../Dependencies ${CMAKE_TEMP_DIR})