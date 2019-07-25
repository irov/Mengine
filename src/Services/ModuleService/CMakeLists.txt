SET(MY_LIB_NAME ModuleService)

ADD_FILTER(
src
	ModuleService.h
	ModuleService.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY(Services)