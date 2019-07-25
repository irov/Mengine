SET(MY_LIB_NAME ResourceService)

ADD_FILTER(
src
	ResourceService.h
	ResourceService.cpp
)

add_definitions(-DMETABUF_EXTERNAL_CONFIG)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/metabuf/include)

ADD_MENGINE_LIBRARY(Services)