
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( vorbis )

SET(TRUE_DIR ../../vorbis)
SET(vorbis_SOURCE_DIR ${TRUE_DIR})

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( vorbis )

set( vorbisfile_LIB_NAME vorbisfile_static )
if(MENGE_USE_MD)
	set( vorbisfile_LIB_NAME ${vorbisfile_LIB_NAME}_md )
endif(MENGE_USE_MD)

# OUR SOURCES
set( vorbis_sources
${TRUE_DIR}/lib/analysis.c
${TRUE_DIR}/lib/bitrate.c
${TRUE_DIR}/lib/block.c
${TRUE_DIR}/lib/codebook.c
${TRUE_DIR}/lib/envelope.c
${TRUE_DIR}/lib/floor0.c
${TRUE_DIR}/lib/floor1.c
${TRUE_DIR}/lib/info.c
${TRUE_DIR}/lib/lookup.c
${TRUE_DIR}/lib/lpc.c
${TRUE_DIR}/lib/lsp.c
${TRUE_DIR}/lib/mapping0.c
${TRUE_DIR}/lib/mdct.c
${TRUE_DIR}/lib/psy.c
${TRUE_DIR}/lib/registry.c
${TRUE_DIR}/lib/res0.c
${TRUE_DIR}/lib/sharedbook.c
${TRUE_DIR}/lib/smallft.c
${TRUE_DIR}/lib/synthesis.c
${TRUE_DIR}/lib/window.c
)

set( vorbisfile_sources
${TRUE_DIR}/lib/vorbisfile.c
)

include_directories(${vorbis_SOURCE_DIR}/include ../../ogg/include)
add_library( ${vorbis_LIB_NAME} STATIC ${vorbis_sources} )
add_library( ${vorbisfile_LIB_NAME} STATIC ${vorbisfile_sources} )
