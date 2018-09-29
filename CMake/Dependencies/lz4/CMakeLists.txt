CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(lz4)

# OUR SOURCES
set( SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/lz4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/lz4hc.c
)

if(WIN32)
    ADD_DEFINITIONS(-DLZ4_FORCE_MEMORY_ACCESS=2)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_library(${PROJECT_NAME} STATIC ${SOURCES})
