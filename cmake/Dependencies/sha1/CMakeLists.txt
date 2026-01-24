CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

project( sha1 )

SET( SOURCE_DIR ${THIRDPARTY_DIR}/sha1 )

# OUR SOURCES
set( sources
${SOURCE_DIR}/sha1.cpp
)

include_directories( ${SOURCE_DIR}/include )

add_library( ${PROJECT_NAME} STATIC ${sources} )
