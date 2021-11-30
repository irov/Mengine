if(MENGINE_LOGGER_DEBUG)
    ADD_DEFINITIONS(-DMENGINE_LOGGER_DEBUG)
endif()

if(MENGINE_BUILD_MENGINE_MASTER_RELEASE)
    ADD_DEFINITIONS(-DMENGINE_MASTER_RELEASE)
endif()

if(MENGINE_BUILD_MENGINE_BUILD_PUBLISH)
    ADD_DEFINITIONS(-DMENGINE_BUILD_PUBLISH)
endif()

if(MENGINE_BUILD_MENGINE_DEVELOPMENT)
    ADD_DEFINITIONS(-DMENGINE_BUILD_DEVELOPMENT)
endif()

if(MENGINE_USE_SCRIPT_SERVICE)
    ADD_DEFINITIONS(-DMENGINE_USE_SCRIPT_SERVICE)
endif()

if(MENGINE_SECURE_VALUE)
    ADD_DEFINITIONS(-DMENGINE_SECURE_VALUE="${MENGINE_SECURE_VALUE}")
endif()

INCLUDE_DIRECTORIES(${MENGINE_SOURCE_DIR})
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/stdex/include)

SET(APPLICATION_COCOAPODS)
SET(APPLICATION_LINK_LIBRARIES)
SET(APPLICATION_DEPENDENCIES)
