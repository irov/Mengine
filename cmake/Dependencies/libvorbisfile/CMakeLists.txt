CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
                                             
project( vorbisfile )

SET( SOURCE_DIR ${THIRDPARTY_DIR}/libvorbis )

# OUR SOURCES
set( vorbisfile_sources
${SOURCE_DIR}/lib/vorbisfile.c
)

add_definitions(-DOV_EXCLUDE_STATIC_CALLBACKS)

include_directories(${SOURCE_DIR}/include ${THIRDPARTY_DIR}/libogg/include)
include_directories(${THIRDPARTY_DIR}/stdex/include)

add_library( ${PROJECT_NAME} STATIC ${vorbisfile_sources} )
