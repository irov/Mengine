CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                            
project(tinyxml)

set( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../tinyxml/tinyxml )

# OUR SOURCES
set(TINYXML_SRCS
	${SOURCE_DIR}/tinyxml.cpp
	${SOURCE_DIR}/tinystr.cpp
	${SOURCE_DIR}/tinyxmlerror.cpp
	${SOURCE_DIR}/tinyxmlparser.cpp
    ${SOURCE_DIR}/tinyxml.h
	${SOURCE_DIR}/tinystr.h
)

include_directories(${SOURCE_DIR}/..)

add_library( ${PROJECT_NAME}_static STATIC ${TINYXML_SRCS} )

