MENGINE_PROJECT(SDLThreadSystem)

ADD_FILTER(
src
	SDLThreadSystem.cpp
	SDLThreadSystem.h
	SDLThreadIdentity.cpp
    SDLThreadIdentity.h
    SDLThreadMutex.h
    SDLThreadMutex.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/SDL)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/SDL2/include)

ADD_MENGINE_LIBRARY(Systems)