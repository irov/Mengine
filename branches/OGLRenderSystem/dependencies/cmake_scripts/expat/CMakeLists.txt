
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( expat )

SET(TRUE_DIR ../../expat/lib )
SET( expat_SOURCE_DIR ../../expat )

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( expat )

# OUR SOURCES
set( expat_sources
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

include_directories(${expat_SOURCE_DIR} ${expat_SOURCE_DIR}/lib)
add_library( ${expat_LIB_NAME} STATIC ${expat_sources} )
