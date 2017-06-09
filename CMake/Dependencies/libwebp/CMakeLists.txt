CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( webp )

SET( SOURCE_DIR ${THIRDPARTY_DIR}/libwebp )

#configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h ${SOURCE_DIR}/src/webp/config.h COPYONLY)

set(sources
    ${SOURCE_DIR}/src/dec/alpha_dec.c
    ${SOURCE_DIR}/src/dec/buffer_dec.c
    ${SOURCE_DIR}/src/dec/frame_dec.c
    ${SOURCE_DIR}/src/dec/idec_dec.c
    ${SOURCE_DIR}/src/dec/io_dec.c
    ${SOURCE_DIR}/src/dec/quant_dec.c
    ${SOURCE_DIR}/src/dec/tree_dec.c
    ${SOURCE_DIR}/src/dec/vp8_dec.c
    ${SOURCE_DIR}/src/dec/vp8l_dec.c
    ${SOURCE_DIR}/src/dec/webp_dec.c
    
    ${SOURCE_DIR}/src/dsp/alpha_processing.c
    ${SOURCE_DIR}/src/dsp/alpha_processing_sse2.c
    ${SOURCE_DIR}/src/dsp/argb.c
    ${SOURCE_DIR}/src/dsp/argb_sse2.c
    ${SOURCE_DIR}/src/dsp/cost.c
    ${SOURCE_DIR}/src/dsp/cost_sse2.c
    ${SOURCE_DIR}/src/dsp/cpu.c
    ${SOURCE_DIR}/src/dsp/dec.c
    ${SOURCE_DIR}/src/dsp/dec_neon.c
    ${SOURCE_DIR}/src/dsp/dec_sse2.c
    ${SOURCE_DIR}/src/dsp/dec_clip_tables.c
    ${SOURCE_DIR}/src/dsp/dec_mips32.c
    ${SOURCE_DIR}/src/dsp/dec_sse41.c
    ${SOURCE_DIR}/src/dsp/lossless.c
    ${SOURCE_DIR}/src/dsp/lossless_sse2.c
    ${SOURCE_DIR}/src/dsp/upsampling.c
    ${SOURCE_DIR}/src/dsp/upsampling_neon.c
    ${SOURCE_DIR}/src/dsp/upsampling_sse2.c
    ${SOURCE_DIR}/src/dsp/yuv.c
    ${SOURCE_DIR}/src/dsp/yuv_sse2.c
    ${SOURCE_DIR}/src/dsp/alpha_processing.c
    ${SOURCE_DIR}/src/dsp/alpha_processing_sse2.c
    ${SOURCE_DIR}/src/dsp/alpha_processing_sse41.c
    ${SOURCE_DIR}/src/dsp/filters.c
    ${SOURCE_DIR}/src/dsp/filters_sse2.c
    ${SOURCE_DIR}/src/dsp/rescaler.c
    ${SOURCE_DIR}/src/dsp/rescaler_sse2.c

    ${SOURCE_DIR}/src/utils/bit_reader_utils.c
    ${SOURCE_DIR}/src/utils/color_cache_utils.c
    ${SOURCE_DIR}/src/utils/filters_utils.c
    ${SOURCE_DIR}/src/utils/huffman_utils.c
    ${SOURCE_DIR}/src/utils/quant_levels_utils.c
    ${SOURCE_DIR}/src/utils/quant_levels_dec_utils.c
    ${SOURCE_DIR}/src/utils/random_utils.c
    ${SOURCE_DIR}/src/utils/rescaler_utils.c
    ${SOURCE_DIR}/src/utils/thread_utils.c
    ${SOURCE_DIR}/src/utils/utils.c    
)

#add_definitions(-DHAVE_CONFIG_H)

# NOW BUILD OUR TARGET
include_directories( ${SOURCE_DIR} ${THIRDPARTY_DIR}/zlib )

add_library( ${PROJECT_NAME} STATIC ${sources} )

