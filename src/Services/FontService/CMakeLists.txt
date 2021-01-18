MENGINE_PROJECT(FontService)

ADD_FILTER(
src
    FontService.cpp
    FontService.h
)

ADD_MENGINE_LIBRARY(Services)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)