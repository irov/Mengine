SET( MY_LIB_NAME SceneService )

ADD_FILTER(
src
	SceneService.h
	SceneService.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY(Services)