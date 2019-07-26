SET(MY_LIB_NAME GraveyardService)

ADD_FILTER(
src
	GraveyardService.cpp
	GraveyardService.h
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY(Services)