CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(zlib)

# OUR SOURCES
set( zlib_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/adler32.c
    ${CMAKE_CURRENT_SOURCE_DIR}/compress.c
    ${CMAKE_CURRENT_SOURCE_DIR}/crc32.c
    ${CMAKE_CURRENT_SOURCE_DIR}/deflate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/infback.c
    ${CMAKE_CURRENT_SOURCE_DIR}/inffast.c
    ${CMAKE_CURRENT_SOURCE_DIR}/inflate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/inftrees.c
    ${CMAKE_CURRENT_SOURCE_DIR}/trees.c
    ${CMAKE_CURRENT_SOURCE_DIR}/uncompr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/zutil.c
)

add_library(${PROJECT_NAME} STATIC ${zlib_sources})
