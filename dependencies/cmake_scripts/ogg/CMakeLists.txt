
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(OGG)

SET(TRUE_DIR ../../ogg)
SET(OGG_SOURCE_DIR ../../ogg)

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS()

# SET LIBNAME
# msvc does not append 'lib' - do it here to have consistent name
if(MSVC)
 set(OGG_LIB_NAME lib)
endif(MSVC)
set( OGG_LIB_NAME ${OGG_LIB_NAME}ogg_static )

# OUR SOURCES
set(libogg_sources
${TRUE_DIR}/include/ogg/ogg.h
${TRUE_DIR}/include/ogg/os_types.h
${TRUE_DIR}/src/bitwise.c
${TRUE_DIR}/src/framing.c
)

include_directories(${OGG_SOURCE_DIR}/include)
add_library( ${OGG_LIB_NAME} STATIC ${libogg_sources} )
