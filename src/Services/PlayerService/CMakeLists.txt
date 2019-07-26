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

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY(Services)