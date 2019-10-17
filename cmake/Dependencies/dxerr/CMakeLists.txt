CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
                                             
project( dxerr )

SET( SOURCE_DIR ${THIRDPARTY_DIR}/../../dxerr )

# OUR SOURCES
set( sources
${SOURCE_DIR}/dxerr15.cpp
)

include_directories( ${SOURCE_DIR}/include )

add_library( ${PROJECT_NAME} STATIC ${sources} )
