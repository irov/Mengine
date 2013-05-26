CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( dlmalloc )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../dlmalloc )

# OUR SOURCES
set( sources
${SOURCE_DIR}/dlmalloc.c
)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../../dlmalloc )

add_library( ${PROJECT_NAME}_static STATIC ${sources} )
