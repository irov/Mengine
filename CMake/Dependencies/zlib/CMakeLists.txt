CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( zlib )

SET( SOURCE_DIR ${THIRDPARTY_DIR}/zlib )

#configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/zconf.h ${SOURCE_DIR}/zconf.h COPYONLY)

# OUR SOURCES
set( zlib_sources
${SOURCE_DIR}/adler32.c
${SOURCE_DIR}/compress.c
${SOURCE_DIR}/crc32.c
${SOURCE_DIR}/deflate.c
${SOURCE_DIR}/infback.c
${SOURCE_DIR}/inffast.c
${SOURCE_DIR}/inflate.c
${SOURCE_DIR}/inftrees.c
${SOURCE_DIR}/trees.c
${SOURCE_DIR}/uncompr.c
${SOURCE_DIR}/zutil.c
)

include_directories(${SOURCE_DIR})
add_library( ${PROJECT_NAME}_static STATIC ${zlib_sources} )
