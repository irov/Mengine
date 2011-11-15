
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( GLee )

SET( CMAKE_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/GLee )

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( GLee )

# OUR SOURCES
set( GLee_sources
	${CMAKE_CURRENT_SOURCE_DIR}/GLee.h
	${CMAKE_CURRENT_SOURCE_DIR}/GLee.c
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library( ${GLee_LIB_NAME} STATIC ${GLee_sources} )
