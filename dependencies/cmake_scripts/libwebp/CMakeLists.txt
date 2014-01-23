CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( webp )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../libwebp )

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h ${SOURCE_DIR}/config.h COPYONLY)

set(sources
    ${SOURCE_DIR}/src/dec/alpha.c
    ${SOURCE_DIR}/src/dec/buffer.c
    ${SOURCE_DIR}/src/dec/frame.c
    ${SOURCE_DIR}/src/dec/idec.c
    ${SOURCE_DIR}/src/dec/io.c
    ${SOURCE_DIR}/src/dec/layer.c
    ${SOURCE_DIR}/src/dec/quant.c
    ${SOURCE_DIR}/src/dec/tree.c
    ${SOURCE_DIR}/src/dec/vp8.c
    ${SOURCE_DIR}/src/dec/vp8l.c
    ${SOURCE_DIR}/src/dec/webp.c    
    
    ${SOURCE_DIR}/src/dsp/cpu.c
    ${SOURCE_DIR}/src/dsp/dec.c
    ${SOURCE_DIR}/src/dsp/dec_neon.c
    ${SOURCE_DIR}/src/dsp/dec_sse2.c
    ${SOURCE_DIR}/src/dsp/lossless.c
    ${SOURCE_DIR}/src/dsp/upsampling.c
    ${SOURCE_DIR}/src/dsp/upsampling_neon.c
    ${SOURCE_DIR}/src/dsp/upsampling_sse2.c
    ${SOURCE_DIR}/src/dsp/yuv.c

    ${SOURCE_DIR}/src/utils/bit_reader.c
    ${SOURCE_DIR}/src/utils/color_cache.c
    ${SOURCE_DIR}/src/utils/filters.c
    ${SOURCE_DIR}/src/utils/huffman.c
    ${SOURCE_DIR}/src/utils/quant_levels_dec.c
    ${SOURCE_DIR}/src/utils/rescaler.c
    ${SOURCE_DIR}/src/utils/thread.c
    ${SOURCE_DIR}/src/utils/utils.c
)

add_definitions(-DHAVE_CONFIG_H)

# NOW BUILD OUR TARGET
include_directories( ${SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../../zlib )

add_library( ${PROJECT_NAME}_static STATIC ${sources} )

