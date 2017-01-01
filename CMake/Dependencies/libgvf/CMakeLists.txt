CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( libgvf )

SET( SOURCE_DIR ${THIRDPARTY_DIR}/ibgvf )


# OUR SOURCES
set( sources
${SOURCE_DIR}/gvf.c
${SOURCE_DIR}/gvf_encoder.c
)

include_directories(${SOURCE_DIR})
include_directories(${THIRDPARTY_DIR}/lz4)

add_library( ${PROJECT_NAME}_static STATIC ${sources} )
