CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( stdex )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../stdex )

# OUR SOURCES
set( sources
${SOURCE_DIR}/src/allocator.cpp
${SOURCE_DIR}/src/const_string.cpp
${SOURCE_DIR}/src/const_string_holder.cpp
${SOURCE_DIR}/src/const_string_holder_none.cpp
)

include_directories( ${SOURCE_DIR}/include )

add_library( ${PROJECT_NAME}_static STATIC ${sources} )
