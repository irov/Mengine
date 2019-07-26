SET(MY_LIB_NAME ThreadService)

ADD_FILTER(
src
	ThreadService.h
	ThreadService.cpp
    ThreadQueue.h
    ThreadQueue.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY(Services)