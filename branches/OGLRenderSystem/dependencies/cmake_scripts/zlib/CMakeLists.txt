
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( zlib )

set( TRUE_DIR ../../zlib)
set( zlib_SOURCE_DIR ${TRUE_DIR})

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( zlib )

# OUR SOURCES
set( zlib_sources
${TRUE_DIR}/crc32.h
${TRUE_DIR}/deflate.h
${TRUE_DIR}/inffast.h
${TRUE_DIR}/inffixed.h
${TRUE_DIR}/inflate.h
${TRUE_DIR}/inftrees.h
${TRUE_DIR}/trees.h
${TRUE_DIR}/zconf.h
${TRUE_DIR}/zlib.h
${TRUE_DIR}/zutil.h
${TRUE_DIR}/adler32.c
${TRUE_DIR}/compress.c
${TRUE_DIR}/crc32.c
${TRUE_DIR}/deflate.c
${TRUE_DIR}/gzio.c
${TRUE_DIR}/infback.c
${TRUE_DIR}/inffast.c
${TRUE_DIR}/inflate.c
${TRUE_DIR}/inftrees.c
${TRUE_DIR}/trees.c
${TRUE_DIR}/uncompr.c
${TRUE_DIR}/zutil.c
)

include_directories(${zlib_SOURCE_DIR})
add_library( ${zlib_LIB_NAME} STATIC ${zlib_sources} )
