CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( pugixml )

SET( SOURCE_DIR ${THIRDPARTY_DIR}/pugixml )

# OUR SOURCES
set( sources
${SOURCE_DIR}/src/pugixml.hpp
${SOURCE_DIR}/src/pugixml.cpp
)

add_definitions(-DPUGIXML_NO_XPATH)

add_library( ${PROJECT_NAME} STATIC ${sources} )
