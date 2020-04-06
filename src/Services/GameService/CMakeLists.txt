MENGINE_PROJECT(GameService)

ADD_FILTER(
src
    GameService.h
    GameService.cpp
    GameServiceAccountProvider.cpp
    GameServiceAccountProvider.h
    GameServiceSoundVolumeProvider.cpp
    GameServiceSoundVolumeProvider.h
)

ADD_MENGINE_LIBRARY(Services)