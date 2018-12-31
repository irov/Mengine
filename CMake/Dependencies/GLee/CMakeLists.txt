CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
                                             
project( GLee )

SET( SOURCE_DIR ${THIRDPARTY_DIR}/GLee )

# OUR SOURCES
set( GLee_sources
	${SOURCE_DIR}/GLee.h
	${SOURCE_DIR}/GLee.c
)

include_directories(${SOURCE_DIR})
add_library( ${PROJECT_NAME} STATIC ${GLee_sources} )
