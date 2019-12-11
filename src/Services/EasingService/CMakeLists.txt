SET(MY_LIB_NAME EasingService)

ADD_FILTER(
src
    EasingHelper.cpp
    EasingHelper.h
    
    EasingService.cpp
    EasingService.h
)

ADD_MENGINE_LIBRARY(Services)