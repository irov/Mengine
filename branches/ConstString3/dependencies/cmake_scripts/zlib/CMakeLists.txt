
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( zlib )

SET( CMAKE_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../zlib )

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( zlib )

# OUR SOURCES
set( zlib_sources
${CMAKE_CURRENT_SOURCE_DIR}/crc32.h
${CMAKE_CURRENT_SOURCE_DIR}/deflate.h
${CMAKE_CURRENT_SOURCE_DIR}/inffast.h
${CMAKE_CURRENT_SOURCE_DIR}/inffixed.h
${CMAKE_CURRENT_SOURCE_DIR}/inflate.h
${CMAKE_CURRENT_SOURCE_DIR}/inftrees.h
${CMAKE_CURRENT_SOURCE_DIR}/trees.h
${CMAKE_CURRENT_SOURCE_DIR}/zconf.h
${CMAKE_CURRENT_SOURCE_DIR}/zlib.h
${CMAKE_CURRENT_SOURCE_DIR}/zutil.h
${CMAKE_CURRENT_SOURCE_DIR}/adler32.c
${CMAKE_CURRENT_SOURCE_DIR}/compress.c
${CMAKE_CURRENT_SOURCE_DIR}/crc32.c
${CMAKE_CURRENT_SOURCE_DIR}/deflate.c
${CMAKE_CURRENT_SOURCE_DIR}/gzclose.c
${CMAKE_CURRENT_SOURCE_DIR}/gzlib.c
${CMAKE_CURRENT_SOURCE_DIR}/gzread.c
${CMAKE_CURRENT_SOURCE_DIR}/gzwrite.c
${CMAKE_CURRENT_SOURCE_DIR}/infback.c
${CMAKE_CURRENT_SOURCE_DIR}/inffast.c
${CMAKE_CURRENT_SOURCE_DIR}/inflate.c
${CMAKE_CURRENT_SOURCE_DIR}/inftrees.c
${CMAKE_CURRENT_SOURCE_DIR}/trees.c
${CMAKE_CURRENT_SOURCE_DIR}/uncompr.c
${CMAKE_CURRENT_SOURCE_DIR}/zutil.c
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library( ${zlib_LIB_NAME} STATIC ${zlib_sources} )
