CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( poly2tri )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../poly2tri )

# OUR SOURCES
set( sources
${SOURCE_DIR}/shapes.cpp
${SOURCE_DIR}/advancing_front.cpp
${SOURCE_DIR}/sweep.cpp
${SOURCE_DIR}/sweep_context.cpp
)

add_library( ${PROJECT_NAME}_static STATIC ${sources} )
