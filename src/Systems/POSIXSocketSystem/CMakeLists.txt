MENGINE_PROJECT(POSIXSocketSystem)

ADD_FILTER(
src
    POSIXSocket.cpp
    POSIXSocket.h
    POSIXDatagram.cpp
    POSIXDatagram.h
    POSIXSocketSystem.cpp
    POSIXSocketSystem.h
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
