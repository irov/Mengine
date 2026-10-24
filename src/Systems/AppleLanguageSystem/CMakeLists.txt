MENGINE_PROJECT(AppleLanguageSystem)

ADD_FILTER(
src
    AppleLanguageSystem.h
    AppleLanguageSystem.mm
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} Apple)
