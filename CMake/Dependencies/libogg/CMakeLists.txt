CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( ogg )

SET( SOURCE_DIR ${THIRDPARTY_DIR}/libogg )

configure_file(Modified/os_types.h ${SOURCE_DIR}/include/ogg/os_types.h COPYONLY)

# OUR SOURCES
set( ogg_sources
${SOURCE_DIR}/src/bitwise.c
${SOURCE_DIR}/src/framing.c
)

include_directories(${SOURCE_DIR}/include)
include_directories(${THIRDPARTY_DIR}/stdex/include)

add_library( ${PROJECT_NAME} STATIC ${ogg_sources} )
