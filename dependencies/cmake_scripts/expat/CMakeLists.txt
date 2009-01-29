
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(Expat)

SET(TRUE_DIR ../../expat/lib)
SET(Expat_SOURCE_DIR ../../expat)

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS()

# SET LIBNAME
# msvc does not append 'lib' - do it here to have consistent name
if(MSVC)
 set(Expat_LIB_NAME lib)
endif(MSVC)
set( Expat_LIB_NAME ${Expat_LIB_NAME}expat_static )

# OUR SOURCES
set(libexpat_sources
${TRUE_DIR}/ascii.h
${TRUE_DIR}/asciitab.h
${TRUE_DIR}/expat.h
${TRUE_DIR}/expat_external.h
${TRUE_DIR}/iasciitab.h
${TRUE_DIR}/internal.h
${TRUE_DIR}/latin1tab.h
${TRUE_DIR}/nametab.h
${TRUE_DIR}/utf8tab.h
${TRUE_DIR}/xmlrole.h
${TRUE_DIR}/xmltok.h
${TRUE_DIR}/xmltok_impl.h
${TRUE_DIR}/xmlparse.c
${TRUE_DIR}/xmlrole.c
${TRUE_DIR}/xmltok.c
${TRUE_DIR}/xmltok_impl.c
${TRUE_DIR}/xmltok_ns.c
)

# SOME NEEDED DEFINITIONS
add_definitions(-DXML_STATIC)
if(WIN32)
  add_definitions(-DCOMPILED_FROM_DSP)
else(APPLE)
 add_definitions(-DHAVE_EXPAT_CONFIG_H)
endif(WIN32)

include_directories(${Expat_SOURCE_DIR} ${Expat_SOURCE_DIR}/lib)
add_library( ${Expat_LIB_NAME} STATIC ${libexpat_sources} )
