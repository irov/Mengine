SET(MY_LIB_NAME PickerService)

ADD_FILTER(
src
	PickerService.h
	PickerService.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY(Services)