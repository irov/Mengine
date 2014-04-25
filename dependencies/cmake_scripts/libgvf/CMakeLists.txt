CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( libgvf )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../libgvf )


# OUR SOURCES
set( sources
${SOURCE_DIR}/gvf.c
${SOURCE_DIR}/gvf_encoder.c
)

include_directories(${SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../lz4)

add_library( ${PROJECT_NAME}_static STATIC ${sources} )
