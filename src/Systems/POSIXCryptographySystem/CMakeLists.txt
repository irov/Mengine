MENGINE_PROJECT(POSIXCryptographySystem)

ADD_FILTER(
src
    POSIXCryptographySystem.cpp
    POSIXCryptographySystem.h
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(${PROJECT_NAME} PRIVATE _GNU_SOURCE)
endif()
