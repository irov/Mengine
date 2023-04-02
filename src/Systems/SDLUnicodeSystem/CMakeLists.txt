MENGINE_PROJECT(SDLUnicodeSystem)

ADD_FILTER(
src
    SDLUnicodeSystem.cpp
    SDLUnicodeSystem.h
)

include_directories(${THIRDPARTY_CONFIG_DIR}/SDL)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL/include)

ADD_MENGINE_LIBRARY(Systems)