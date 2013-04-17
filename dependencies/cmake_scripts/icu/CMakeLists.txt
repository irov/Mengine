
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( icu )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../icu )

# OUR SOURCES
set( sources
${SOURCE_DIR}/source/common/ustring.cpp
${SOURCE_DIR}/source/common/ustrtrns.cpp

${SOURCE_DIR}/source/common/cmemory.c
${SOURCE_DIR}/source/common/cstring.c
${SOURCE_DIR}/source/common/uinvchar.c
${SOURCE_DIR}/source/common/udataswp.c
${SOURCE_DIR}/source/common/utf_impl.c
${SOURCE_DIR}/source/common/utypes.c
${SOURCE_DIR}/source/common/ustr_wcs.cpp
)

add_definitions(-DU_STATIC_IMPLEMENTATION)

include_directories( ${SOURCE_DIR}/source/common )

add_library( ${PROJECT_NAME}_static STATIC ${sources} )
