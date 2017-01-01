CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( libvpx )

SET( SOURCE_DIR ${THIRDPARTY_DIR}/libvpx )

IF(MSVC)
    configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/vs10/vpx_config.h ${SOURCE_DIR}/vpx_config.h COPYONLY)
ENDIF(MSVC)

IF(MINGW)
    configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/mingw/vpx_config.h ${SOURCE_DIR}/vpx_config.h COPYONLY)
ENDIF(MINGW)


# OUR SOURCES
set( sources
    ${SOURCE_DIR}/vpx/src/vpx_decoder.c
    ${SOURCE_DIR}/vpx/src/vpx_encoder.c
    ${SOURCE_DIR}/vpx/src/vpx_codec.c
    ${SOURCE_DIR}/vpx/src/vpx_image.c
    
    ${SOURCE_DIR}/vpx_mem/vpx_mem.c
    
    ${SOURCE_DIR}/vpx_scale/generic/vpx_scale.c
    ${SOURCE_DIR}/vpx_scale/generic/yv12config.c
    ${SOURCE_DIR}/vpx_scale/generic/yv12extend.c
    ${SOURCE_DIR}/vpx_scale/generic/gen_scalers.c
    ${SOURCE_DIR}/vpx_scale/vpx_scale_asm_offsets.c
    ${SOURCE_DIR}/vpx_scale/vpx_scale_rtcd.c
    
    ${SOURCE_DIR}/vp8/common/alloccommon.c
    ${SOURCE_DIR}/vp8/common/blockd.c
    ${SOURCE_DIR}/vp8/common/debugmodes.c
    ${SOURCE_DIR}/vp8/common/dequantize.c
    ${SOURCE_DIR}/vp8/common/entropy.c
    ${SOURCE_DIR}/vp8/common/entropymode.c
    ${SOURCE_DIR}/vp8/common/entropymv.c
    ${SOURCE_DIR}/vp8/common/extend.c
    ${SOURCE_DIR}/vp8/common/filter.c
    ${SOURCE_DIR}/vp8/common/findnearmv.c
    ${SOURCE_DIR}/vp8/common/generic/systemdependent.c
    ${SOURCE_DIR}/vp8/common/idct_blk.c
    ${SOURCE_DIR}/vp8/common/idctllm.c
    ${SOURCE_DIR}/vp8/common/rtcd.c
    ${SOURCE_DIR}/vp8/common/loopfilter.c
    ${SOURCE_DIR}/vp8/common/loopfilter_filters.c
    ${SOURCE_DIR}/vp8/common/mbpitch.c
    ${SOURCE_DIR}/vp8/common/modecont.c
    ${SOURCE_DIR}/vp8/common/quant_common.c
    ${SOURCE_DIR}/vp8/common/reconinter.c    
    ${SOURCE_DIR}/vp8/common/reconintra.c
    ${SOURCE_DIR}/vp8/common/reconintra4x4.c
    ${SOURCE_DIR}/vp8/common/sad_c.c
    ${SOURCE_DIR}/vp8/common/setupintrarecon.c
    ${SOURCE_DIR}/vp8/common/swapyv12buffer.c
    ${SOURCE_DIR}/vp8/common/variance_c.c
    ${SOURCE_DIR}/vp8/common/treecoder.c
    
    ${SOURCE_DIR}/vp8/vp8_dx_iface.c
    
    ${SOURCE_DIR}/vp8/decoder/dboolhuff.c
    ${SOURCE_DIR}/vp8/decoder/decodemv.c
    ${SOURCE_DIR}/vp8/decoder/decodframe.c
    ${SOURCE_DIR}/vp8/decoder/detokenize.c
    ${SOURCE_DIR}/vp8/decoder/onyxd_if.c
    
    ${SOURCE_DIR}/vpx_config.c
    )

include_directories( ${SOURCE_DIR} )

add_library( ${PROJECT_NAME}_static STATIC ${sources} )
