SET(MY_LIB_NAME WatchdogService)

ADD_FILTER(
src
	WatchdogService.h
	WatchdogService.cpp
)

ADD_MENGINE_LIBRARY(Services)