SET(MY_LIB_NAME GameService)

ADD_FILTER(
src
	GameService.h
	GameService.cpp
    GameServiceAccountProvider.cpp
    GameServiceAccountProvider.h
    GameServiceSoundVolumeProvider.cpp
    GameServiceSoundVolumeProvider.h
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY(Services)


