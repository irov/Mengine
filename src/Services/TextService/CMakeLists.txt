SET(MY_LIB_NAME TextService)

ADD_FILTER(
src
    TextService.cpp
    TextService.h
	TextEntry.cpp
    TextEntry.h
    TextLocalePak.cpp
    TextLocalePak.h
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/utf8/source)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY(Services)