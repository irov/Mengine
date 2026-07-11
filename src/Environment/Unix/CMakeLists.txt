MENGINE_PROJECT(Unix)

ADD_FILTER(
src
    UnixIncluder.h
    UnixPlatformServiceExtensionInterface.h

    UnixEnvironmentService.h
    UnixEnvironmentService.cpp
)

ADD_MENGINE_LIBRARY(Environment)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
