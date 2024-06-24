MENGINE_PROJECT(AndroidCryptographySystem)

ADD_FILTER(
src
    AndroidCryptographySystem.h
    AndroidCryptographySystem.cpp
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Android)