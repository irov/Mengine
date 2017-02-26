CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

project(SDL2_net)

SET(SOURCE_DIR ${THIRDPARTY_DIR}/SDL2_net)

# OUR SOURCES
set(SOURCES
    ${SOURCE_DIR}/SDLnet.c
    ${SOURCE_DIR}/SDL_net.h
    ${SOURCE_DIR}/SDLnetTCP.c
    ${SOURCE_DIR}/SDLnetUDP.c
    ${SOURCE_DIR}/SDLnetselect.c
)

include_directories(${SOURCE_DIR}/lib)
include_directories(${THIRDPARTY_DIR}/SDL2/include)

SET(FORCE_STATIC_VCRT ON CACHE BOOL "Force /MT for static VC runtimes" FORCE)
add_library(${PROJECT_NAME} STATIC ${SOURCES})
