CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(Depends_Android)

SET(MENGINE_REPOSITORY ${CMAKE_CURRENT_LIST_DIR}/../..)

INCLUDE(${MENGINE_REPOSITORY}/cmake/build_options_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/base_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/macro_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/android_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/dependencies_template.cmake)

SET_MENGINE_ENVIRONMENT(ANDROID OPENGL ANDROID GRADLE)

SET_MENGINE_DEPENDENCIES_OUTPUT_DIRECTORY()

SET(MENGINE_DEPENDENCE_RPMALLOC OFF CACHE BOOL "MENGINE_DEPENDENCE_RPMALLOC" FORCE)
SET(MENGINE_DEPENDENCE_OPENAL_SOFT OFF CACHE BOOL "MENGINE_DEPENDENCE_OPENAL_SOFT" FORCE)
SET(MENGINE_DEPENDENCE_SDL2 OFF CACHE BOOL "MENGINE_DEPENDENCE_SDL2" FORCE)
SET(MENGINE_DEPENDENCE_OZZ OFF CACHE BOOL "MENGINE_DEPENDENCE_OZZ" FORCE)

ADD_SUBDIRECTORY(${CMAKE_CURRENT_LIST_DIR}/../Dependencies ${CMAKE_TEMP_DIR})