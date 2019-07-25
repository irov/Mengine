SET(MY_LIB_NAME InputService)

ADD_FILTER(
src
	InputService.cpp
	InputService.h
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY(Services)