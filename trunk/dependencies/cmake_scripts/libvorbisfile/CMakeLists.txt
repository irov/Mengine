CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( vorbisfile )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../libvorbis )

# OUR SOURCES
set( vorbisfile_sources
${SOURCE_DIR}/lib/vorbisfile.c
)

include_directories(${SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/../../libogg/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../stdex/include)

add_library( ${PROJECT_NAME}_static STATIC ${vorbisfile_sources} )
