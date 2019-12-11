SET(MY_LIB_NAME TimepipeService)

ADD_FILTER(
src
    TimepipeService.h
    TimepipeService.cpp
)

ADD_MENGINE_LIBRARY(Services)