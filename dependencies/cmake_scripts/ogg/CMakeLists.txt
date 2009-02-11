
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( ogg )

SET(TRUE_DIR ../../ogg)
SET(ogg_SOURCE_DIR ../../ogg)

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( ogg )

# OUR SOURCES
set( ogg_sources
${TRUE_DIR}/include/ogg/ogg.h
${TRUE_DIR}/include/ogg/os_types.h
${TRUE_DIR}/src/bitwise.c
${TRUE_DIR}/src/framing.c
)

include_directories(${ogg_SOURCE_DIR}/include)
add_library( ${ogg_LIB_NAME} STATIC ${ogg_sources} )
