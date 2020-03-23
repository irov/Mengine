CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
                                             
project( vorbis )

SET( SOURCE_DIR ${THIRDPARTY_DIR}/libvorbis )

# OUR SOURCES
set( vorbis_sources
${SOURCE_DIR}/lib/analysis.c
${SOURCE_DIR}/lib/bitrate.c
${SOURCE_DIR}/lib/block.c
${SOURCE_DIR}/lib/codebook.c
${SOURCE_DIR}/lib/envelope.c
${SOURCE_DIR}/lib/floor0.c
${SOURCE_DIR}/lib/floor1.c
${SOURCE_DIR}/lib/info.c
${SOURCE_DIR}/lib/lookup.c
${SOURCE_DIR}/lib/lpc.c
${SOURCE_DIR}/lib/lsp.c
${SOURCE_DIR}/lib/mapping0.c
${SOURCE_DIR}/lib/mdct.c
${SOURCE_DIR}/lib/psy.c
${SOURCE_DIR}/lib/registry.c
${SOURCE_DIR}/lib/res0.c
${SOURCE_DIR}/lib/sharedbook.c
${SOURCE_DIR}/lib/smallft.c
${SOURCE_DIR}/lib/synthesis.c
${SOURCE_DIR}/lib/window.c
)

include_directories(${SOURCE_DIR}/include)
include_directories(${THIRDPARTY_DIR}/libogg/include)

add_library( ${PROJECT_NAME} STATIC ${vorbis_sources} )
