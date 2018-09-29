CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(poly2tri)

# OUR SOURCES
set( sources
    ${CMAKE_CURRENT_SOURCE_DIR}/common/shapes.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/sweep/advancing_front.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/sweep/cdt.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/sweep/sweep.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/sweep/sweep_context.cc
)

add_library( ${PROJECT_NAME} STATIC ${sources} )
