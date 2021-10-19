MENGINE_PROJECT(ConfigService)

ADD_FILTER(
src
    ConfigService.h
    ConfigService.cpp

    MemoryConfig.h
    MemoryConfig.cpp
    MultiConfig.h
    MultiConfig.cpp
)

ADD_MENGINE_LIBRARY(Services)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})