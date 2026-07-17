MENGINE_PROJECT(SocketService)

ADD_FILTER(
src
    SocketService.cpp
    SocketService.h
)

ADD_MENGINE_LIBRARY(Services)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
