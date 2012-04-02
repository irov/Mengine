
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( AtlasAllocator )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../AtlasAllocator )

# OUR SOURCES
set( sources
${SOURCE_DIR}/src/AtlasAllocator.h
${SOURCE_DIR}/src/AtlasAllocator.cpp
)

add_library( ${PROJECT_NAME}_static STATIC ${sources} )
