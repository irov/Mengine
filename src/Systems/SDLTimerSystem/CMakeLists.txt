MENGINE_PROJECT(SDLTimerSystem)

ADD_FILTER(
src
    SDLTimerSystem.h
    SDLTimerSystem.cpp
)

include_directories(${THIRDPARTY_CONFIG_DIR}/SDL)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include)

ADD_MENGINE_LIBRARY(Systems)