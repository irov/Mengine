
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project(vorbis)

SET(TRUE_DIR ../../vorbis)
SET(vorbis_SOURCE_DIR ${TRUE_DIR})

INCLUDE( ../menge_macro.cmake )

MENGE_SET_PROJECT_DEFAULTS()

# SET LIBNAME
# msvc does not append 'lib' - do it here to have consistent name
if(MSVC)
 set(vorbis_LIB_NAME lib)
 set(vorbisfile_LIB_NAME lib)
endif(MSVC)
set( vorbis_LIB_NAME ${vorbis_LIB_NAME}vorbis_static )
set( vorbisfile_LIB_NAME ${vorbisfile_LIB_NAME}vorbisfile_static )

# OUR SOURCES
set(libvorbis_sources
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

set(libvorbisfile_sources
${TRUE_DIR}/lib/vorbisfile.c
)

include_directories(${vorbis_SOURCE_DIR}/include ../../ogg/include)
add_library( ${vorbis_LIB_NAME} STATIC ${libvorbis_sources} )
add_library( ${vorbisfile_LIB_NAME} STATIC ${libvorbisfile_sources} )
