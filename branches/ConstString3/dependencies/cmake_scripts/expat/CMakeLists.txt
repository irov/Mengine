
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( expat )

SET( CMAKE_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../expat/lib )
SET( expat_SOURCE_DIR ../../expat )

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( expat )

# OUR SOURCES
set( expat_sources
${CMAKE_CURRENT_SOURCE_DIR}/ascii.h
${CMAKE_CURRENT_SOURCE_DIR}/asciitab.h
${CMAKE_CURRENT_SOURCE_DIR}/expat.h
${CMAKE_CURRENT_SOURCE_DIR}/expat_external.h
${CMAKE_CURRENT_SOURCE_DIR}/iasciitab.h
${CMAKE_CURRENT_SOURCE_DIR}/internal.h
${CMAKE_CURRENT_SOURCE_DIR}/latin1tab.h
${CMAKE_CURRENT_SOURCE_DIR}/nametab.h
${CMAKE_CURRENT_SOURCE_DIR}/utf8tab.h
${CMAKE_CURRENT_SOURCE_DIR}/xmlrole.h
${CMAKE_CURRENT_SOURCE_DIR}/xmltok.h
${CMAKE_CURRENT_SOURCE_DIR}/xmltok_impl.h
${CMAKE_CURRENT_SOURCE_DIR}/xmlparse.c
${CMAKE_CURRENT_SOURCE_DIR}/xmlrole.c
${CMAKE_CURRENT_SOURCE_DIR}/xmltok.c
${CMAKE_CURRENT_SOURCE_DIR}/xmltok_impl.c
${CMAKE_CURRENT_SOURCE_DIR}/xmltok_ns.c
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

