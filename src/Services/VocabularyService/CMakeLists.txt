SET(MY_LIB_NAME VocabularyService)

ADD_FILTER(
src
	VocabularyService.h
	VocabularyService.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY(Services)