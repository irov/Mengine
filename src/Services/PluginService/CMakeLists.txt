SET(MY_LIB_NAME PluginService)

ADD_FILTER(
src
	PluginService.h
	PluginService.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY(Services)