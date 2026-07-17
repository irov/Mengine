MENGINE_PROJECT(Win32SocketSystem)

ADD_FILTER(
src
    Win32Socket.cpp
    Win32Socket.h
    Win32Datagram.cpp
    Win32Datagram.h
    Win32SocketSystem.cpp
    Win32SocketSystem.h
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel ws2_32)
