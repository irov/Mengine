CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( crunch )

SET( SOURCE_DIR ${THIRDPARTY_DIR}/crunch )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/crn_decomp_ext.h ${SOURCE_DIR}/inc/crn_decomp_ext.h COPYONLY)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/crn_decomp_ext.cpp ${SOURCE_DIR}/inc/crn_decomp_ext.cpp COPYONLY)

# OUR SOURCES
set( sources
    ${SOURCE_DIR}/inc/crn_decomp_ext.cpp
)

include_directories(${SOURCE_DIR})

add_library( ${PROJECT_NAME} STATIC ${sources} )
