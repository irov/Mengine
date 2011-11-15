
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( ogg )

SET( CMAKE_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/ogg )

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( ogg )

# OUR SOURCES
set( ogg_sources
${CMAKE_CURRENT_SOURCE_DIR}/include/ogg/ogg.h
${CMAKE_CURRENT_SOURCE_DIR}/include/ogg/os_types.h
${CMAKE_CURRENT_SOURCE_DIR}/src/bitwise.c
${CMAKE_CURRENT_SOURCE_DIR}/src/framing.c
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_library( ${ogg_LIB_NAME} STATIC ${ogg_sources} )
