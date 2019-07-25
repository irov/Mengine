set(MY_LIB_NAME SDLUnicodeSystem)

ADD_FILTER(
src
    SDLUnicodeSystem.cpp
    SDLUnicodeSystem.h
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include)

ADD_MENGINE_LIBRARY(Systems)