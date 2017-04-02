CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(metabuf)

SET( SOURCE_DIR ${THIRDPARTY_DIR}/metabuf )

configure_file(Metaconfig.hpp ${SOURCE_DIR}/metabuf/Metaconfig.hpp COPYONLY)

set( sources
    ${SOURCE_DIR}/metabuf/Metabuf.cpp
    )
    
include_directories(${SOURCE_DIR})
include_directories(${THIRDPARTY_DIR}/stdex/include)         

add_library( ${PROJECT_NAME} STATIC ${sources} )