SET(MY_LIB_NAME StringizeService)

ADD_FILTER(
src
	StringizeService.h
	StringizeService.cpp
    ConstStringHolderMemory.h
    ConstStringHolderMemory.cpp
)

ADD_MENGINE_LIBRARY(Services)