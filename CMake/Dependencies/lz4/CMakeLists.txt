CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( lz4 )

SET( SOURCE_DIR ${THIRDPARTY_DIR}/lz4 )

# OUR SOURCES
set( SOURCES
${SOURCE_DIR}/lib/lz4.c
${SOURCE_DIR}/lib/lz4hc.c
)

include_directories(${SOURCE_DIR}/lib)

add_library( ${PROJECT_NAME}_static STATIC ${SOURCES} )
