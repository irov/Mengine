
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( ogg )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../ogg )

# OUR SOURCES
set( ogg_sources
${SOURCE_DIR}/include/ogg/ogg.h
${SOURCE_DIR}/include/ogg/os_types.h
${SOURCE_DIR}/src/bitwise.c
${SOURCE_DIR}/src/framing.c
)

include_directories(${SOURCE_DIR}/include)

add_library( ${PROJECT_NAME}_static STATIC ${ogg_sources} )
