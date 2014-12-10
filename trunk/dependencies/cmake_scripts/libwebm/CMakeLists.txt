CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( webm )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../libwebm )

set(sources
    ${SOURCE_DIR}/mkvmuxer.cpp
    ${SOURCE_DIR}/mkvmuxerutil.cpp
    ${SOURCE_DIR}/mkvparser.cpp
    ${SOURCE_DIR}/mkvreader.cpp
)

add_library( ${PROJECT_NAME}_static STATIC ${sources} )

