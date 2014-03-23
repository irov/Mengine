CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( lz4 )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../lz4 )

# OUR SOURCES
set( zlib_sources
${SOURCE_DIR}/lz4.c
${SOURCE_DIR}/lz4hc.c
)

include_directories(${SOURCE_DIR})
add_library( ${PROJECT_NAME}_static STATIC ${zlib_sources} )
