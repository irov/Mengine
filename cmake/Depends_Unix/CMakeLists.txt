CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

PROJECT(Depends_Unix)

SET(MENGINE_REPOSITORY ${CMAKE_CURRENT_LIST_DIR}/../..)

INCLUDE(${MENGINE_REPOSITORY}/cmake/build_options_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/base_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/macro_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/unix_template.cmake)
INCLUDE(${MENGINE_REPOSITORY}/cmake/dependencies_template.cmake)

SET_MENGINE_ENVIRONMENT(UNIX OPENGL SDL2 GCC)

SET_MENGINE_DEPENDENCIES_OUTPUT_DIRECTORY()

SET(MENGINE_DEPENDENCE_WEBP OFF CACHE BOOL "MENGINE_DEPENDENCE_WEBP" FORCE)
SET(MENGINE_DEPENDENCE_IMGUI OFF CACHE BOOL "MENGINE_DEPENDENCE_IMGUI" FORCE)
SET(MENGINE_DEPENDENCE_OZZ OFF CACHE BOOL "MENGINE_DEPENDENCE_OZZ" FORCE)
SET(MENGINE_DEPENDENCE_RPMALLOC OFF CACHE BOOL "MENGINE_DEPENDENCE_RPMALLOC" FORCE)
SET(MENGINE_DEPENDENCE_SENTRY OFF CACHE BOOL "MENGINE_DEPENDENCE_SENTRY" FORCE)

ADD_SUBDIRECTORY(${CMAKE_CURRENT_LIST_DIR}/../Dependencies ${CMAKE_TEMP_DIR})