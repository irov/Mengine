CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(vorbisfile)

# OUR SOURCES
set( vorbisfile_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/vorbisfile.c
)

add_definitions(-DOV_EXCLUDE_STATIC_CALLBACKS)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${THIRDPARTY_DIR}/libogg/include)
include_directories(${THIRDPARTY_DIR}/stdex/include)

add_library(${PROJECT_NAME} STATIC ${vorbisfile_sources})
