
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(OGG)

SET(TRUE_DIR ../../ogg)
SET(OGG_SOURCE_DIR ../../ogg)

# SET LIBNAME
# msvc does not append 'lib' - do it here to have consistent name
if(MSVC)
 set(OGG_LIB_NAME lib)
endif(MSVC)
set( OGG_LIB_NAME ${OGG_LIB_NAME}ogg_static )

# to distinguish between debug and release lib
set(CMAKE_DEBUG_POSTFIX "d")

# OUR SOURCES
set(libogg_sources
${TRUE_DIR}/include/ogg/ogg.h
${TRUE_DIR}/include/ogg/os_types.h
${TRUE_DIR}/src/bitwise.c
${TRUE_DIR}/src/framing.c
)

# SOME NEEDED DEFINITIONS
if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif(MSVC)

include_directories(${OGG_SOURCE_DIR}/include)
add_library( ${OGG_LIB_NAME} STATIC ${libogg_sources} )

#install_targets(/lib ${OGG_LIB_NAME})

