SET(MY_LIB_NAME PlayerService)

ADD_FILTER(
src
	PlayerService.h
	PlayerService.cpp    
    PlayerGlobalAffectorable.h
    PlayerGlobalAffectorable.cpp
    PlayerGlobalInputHandler.h
    PlayerGlobalInputHandler.cpp
)

ADD_MENGINE_LIBRARY(Services)