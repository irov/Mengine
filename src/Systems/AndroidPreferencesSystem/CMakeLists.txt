MENGINE_PROJECT(AndroidPreferencesSystem)

ADD_FILTER(
src
    AndroidPreferencesSystem.h
    AndroidPreferencesSystem.cpp
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Android)