CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project( ae )

SET( SOURCE_DIR ${THIRDPARTY_DIR}/AE )

# OUR SOURCES
set( sources
${SOURCE_DIR}/Util/AEGP_SuiteHandler.cpp
${SOURCE_DIR}/Util/MissingSuiteError.cpp
)

include_directories( ${SOURCE_DIR}/Util )
include_directories( ${SOURCE_DIR}/Headers )
include_directories( ${SOURCE_DIR}/Headers/SP )
include_directories( ${SOURCE_DIR}/Headers/Win )

add_library( ${PROJECT_NAME} STATIC ${sources} )
