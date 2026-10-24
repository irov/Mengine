MENGINE_PROJECT(AndroidLanguageSystem)

ADD_FILTER(
src
    AndroidLanguageSystem.h
    AndroidLanguageSystem.cpp
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Android)
