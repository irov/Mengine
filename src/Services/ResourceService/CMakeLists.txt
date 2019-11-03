SET(MY_LIB_NAME ResourceService)

ADD_FILTER(
src
    ResourceBank.h
    ResourceBank.cpp
	ResourceService.h
	ResourceService.cpp
)

ADD_MENGINE_LIBRARY(Services)