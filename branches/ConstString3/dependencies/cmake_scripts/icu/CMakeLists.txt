
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( icu )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../icu/source/common )

# OUR SOURCES
set( sources
${SOURCE_DIR}/ustring.cpp
${SOURCE_DIR}/ustrtrns.cpp

${SOURCE_DIR}/cmemory.c
${SOURCE_DIR}/cstring.c
${SOURCE_DIR}/uinvchar.c
${SOURCE_DIR}/udataswp.c
${SOURCE_DIR}/utf_impl.c
)

add_definitions(-DU_STATIC_IMPLEMENTATION)

add_library( ${PROJECT_NAME}_static STATIC ${sources} )
