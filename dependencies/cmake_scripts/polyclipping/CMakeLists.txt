CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( polyclipping )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../polyclipping )

# OUR SOURCES
set( sources
${SOURCE_DIR}/clipper.cpp
)

add_library( ${PROJECT_NAME}_static STATIC ${sources} )
