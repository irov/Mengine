
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( vorbis )

SET( CMAKE_CURRENT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/../vorbis )


INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS( vorbis )

set( vorbisfile_LIB_NAME vorbisfile_static )
if(MENGE_USE_MT)
	set( vorbisfile_LIB_NAME ${vorbisfile_LIB_NAME}_mt )
endif(MENGE_USE_MT)

# OUR SOURCES
set( vorbis_sources
${CMAKE_CURRENT_SOURCE_DIR}/lib/analysis.c
${CMAKE_CURRENT_SOURCE_DIR}/lib/bitrate.c
${CMAKE_CURRENT_SOURCE_DIR}/lib/block.c
${CMAKE_CURRENT_SOURCE_DIR}/lib/codebook.c
${CMAKE_CURRENT_SOURCE_DIR}/lib/envelope.c
${CMAKE_CURRENT_SOURCE_DIR}/lib/floor0.c
${CMAKE_CURRENT_SOURCE_DIR}/lib/floor1.c
${CMAKE_CURRENT_SOURCE_DIR}/lib/info.c
${CMAKE_CURRENT_SOURCE_DIR}/lib/lookup.c
${CMAKE_CURRENT_SOURCE_DIR}/lib/lpc.c
${CMAKE_CURRENT_SOURCE_DIR}/lib/lsp.c
${CMAKE_CURRENT_SOURCE_DIR}/lib/mapping0.c
${CMAKE_CURRENT_SOURCE_DIR}/lib/mdct.c
${CMAKE_CURRENT_SOURCE_DIR}/lib/psy.c
${CMAKE_CURRENT_SOURCE_DIR}/lib/registry.c
${CMAKE_CURRENT_SOURCE_DIR}/lib/res0.c
${CMAKE_CURRENT_SOURCE_DIR}/lib/sharedbook.c
${CMAKE_CURRENT_SOURCE_DIR}/lib/smallft.c
${CMAKE_CURRENT_SOURCE_DIR}/lib/synthesis.c
${CMAKE_CURRENT_SOURCE_DIR}/lib/window.c
)

set( vorbisfile_sources
${CMAKE_CURRENT_SOURCE_DIR}/lib/vorbisfile.c
)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/../ogg/include )
add_library( ${vorbis_LIB_NAME} STATIC ${vorbis_sources} )
add_library( ${vorbisfile_LIB_NAME} STATIC ${vorbisfile_sources} )
