SET(MY_LIB_NAME ConfigService)

ADD_FILTER(
src
    IniUtil.h
    IniUtil.cpp
    IniConfig.h
    IniConfig.cpp
    MemoryConfig.h
    MemoryConfig.cpp
	ConfigService.h
	ConfigService.cpp
)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/tinyini/include)

ADD_MENGINE_LIBRARY(Services)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}tinyini${MENGINE_LIB_SUFFIX})