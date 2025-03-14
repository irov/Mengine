MENGINE_PROJECT(AppleCryptographySystem)

ADD_FILTER(
src
    AppleCryptographySystem.h
    AppleCryptographySystem.mm
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Apple)