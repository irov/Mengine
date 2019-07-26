SET(MY_LIB_NAME PrototypeService)

ADD_FILTER(
src
	PrototypeService.h
	PrototypeService.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY(Services)