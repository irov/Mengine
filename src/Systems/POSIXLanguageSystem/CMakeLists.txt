MENGINE_PROJECT(POSIXLanguageSystem)

ADD_FILTER(
src
    POSIXLanguageSystem.h
    POSIXLanguageSystem.cpp
)

ADD_MENGINE_LIBRARY(Systems)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)
