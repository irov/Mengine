SET(CMAKE_TEMP_DIR ${CMAKE_BINARY_DIR}/solutions)

if(MENGINE_BUILD_MENGINE_MASTER_RELEASE)
    ADD_DEFINITIONS(-DMENGINE_MASTER_RELEASE)
endif()

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_DEFINITIONS(-DMENGINE_USE_SCRIPT_SERVICE)
endif()

INCLUDE_DIRECTORIES(${MENGINE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/stdex/include)

SET(APPLICATION_DEPENDENCIES)
SET(APPLICATION_LINK_LIBRARIES)
