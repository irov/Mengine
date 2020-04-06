MENGINE_PROJECT(SDLTimerSystem)

ADD_FILTER(
src
    SDLTimerSystem.h
    SDLTimerSystem.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include)

ADD_MENGINE_LIBRARY(Systems)