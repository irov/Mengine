MENGINE_PROJECT(ConfigService)

ADD_FILTER(
src
    ConfigService.h
    ConfigService.cpp

    PersistentConfig.h
    PersistentConfig.cpp
    MemoryConfig.h
    MemoryConfig.cpp
    MultiConfig.h
    MultiConfig.cpp
)

ADD_MENGINE_LIBRARY(Services)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)