SET(MY_LIB_NAME LoaderService)

ADD_FILTER(
src
	LoaderService.cpp
	LoaderService.h
)

add_definitions(-DMETABUF_EXTERNAL_CONFIG)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)

ADD_MENGINE_LIBRARY(Services)