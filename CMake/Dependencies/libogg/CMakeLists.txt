CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(ogg) 

configure_file(${PROJECT_CONFIGURE_DIR}/os_types.h ${CMAKE_CURRENT_SOURCE_DIR}/include/ogg/os_types.h COPYONLY)

# OUR SOURCES
set( ogg_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bitwise.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/framing.c
)

include_directories(${SOURCE_DIR}/include)
include_directories(${THIRDPARTY_DIR}/stdex/include)

add_library(${PROJECT_NAME} STATIC ${ogg_sources})
