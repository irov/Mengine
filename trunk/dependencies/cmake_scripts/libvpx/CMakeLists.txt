CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
                                             
project( libvpx )

SET( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../libvpx )

IF(MSVC)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/vs10/vpx_version.h ${SOURCE_DIR}/vpx_version.h COPYONLY)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/vs10/vpx_config.h ${SOURCE_DIR}/vpx_config.h COPYONLY)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/vs10/vpx_config.c ${SOURCE_DIR}/vpx_config.c COPYONLY)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/vs10/vp9_rtcd.h ${SOURCE_DIR}/vp9_rtcd.h COPYONLY)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/vs10/vpx_scale_rtcd.h ${SOURCE_DIR}/vpx_scale_rtcd.h COPYONLY)
ENDIF(MSVC)

IF(MINGW)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/mingw/vpx_version.h ${SOURCE_DIR}/vpx_version.h COPYONLY)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/mingw/vpx_config.h ${SOURCE_DIR}/vpx_config.h COPYONLY)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/mingw/vpx_config.c ${SOURCE_DIR}/vpx_config.c COPYONLY)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/mingw/vp9_rtcd.h ${SOURCE_DIR}/vp9_rtcd.h COPYONLY)
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/mingw/vpx_scale_rtcd.h ${SOURCE_DIR}/vpx_scale_rtcd.h COPYONLY)
ENDIF(MINGW)


# OUR SOURCES
set( sources    
    ${SOURCE_DIR}/vpx/src/vpx_decoder.c    
    ${SOURCE_DIR}/vpx/src/vpx_codec.c
    ${SOURCE_DIR}/vpx/src/vpx_image.c
    ${SOURCE_DIR}/vpx/src/vpx_encoder.c
    ${SOURCE_DIR}/vpx_mem/vpx_mem.c
    ${SOURCE_DIR}/vpx_scale/generic/vpx_scale.c
    ${SOURCE_DIR}/vpx_scale/generic/yv12config.c
    ${SOURCE_DIR}/vpx_scale/generic/yv12extend.c
    ${SOURCE_DIR}/vpx_scale/generic/gen_scalers.c
    ${SOURCE_DIR}/vpx_scale/vpx_scale_asm_offsets.c
    ${SOURCE_DIR}/vpx_scale/vpx_scale_rtcd.c
    ${SOURCE_DIR}/vpx_ports/x86_cpuid.c    
    ${SOURCE_DIR}/vp9/common/vp9_alloccommon.c
    ${SOURCE_DIR}/vp9/common/vp9_convolve.c
    ${SOURCE_DIR}/vp9/common/vp9_debugmodes.c
    ${SOURCE_DIR}/vp9/common/vp9_entropy.c
    ${SOURCE_DIR}/vp9/common/vp9_entropymode.c
    ${SOURCE_DIR}/vp9/common/vp9_entropymv.c
    ${SOURCE_DIR}/vp9/common/vp9_extend.c
    ${SOURCE_DIR}/vp9/common/vp9_filter.c
    ${SOURCE_DIR}/vp9/common/vp9_findnearmv.c
    ${SOURCE_DIR}/vp9/common/generic/vp9_systemdependent.c
    ${SOURCE_DIR}/vp9/common/vp9_idct.c
    ${SOURCE_DIR}/vp9/common/vp9_pred_common.c
    ${SOURCE_DIR}/vp9/common/vp9_seg_common.c
    ${SOURCE_DIR}/vp9/common/vp9_tile_common.c
    ${SOURCE_DIR}/vp9/common/vp9_loopfilter.c
    ${SOURCE_DIR}/vp9/common/vp9_loopfilter_filters.c
    ${SOURCE_DIR}/vp9/common/vp9_mvref_common.c
    ${SOURCE_DIR}/vp9/common/vp9_quant_common.c
    ${SOURCE_DIR}/vp9/common/vp9_reconinter.c
    ${SOURCE_DIR}/vp9/common/vp9_reconintra.c
    ${SOURCE_DIR}/vp9/common/vp9_treecoder.c
    ${SOURCE_DIR}/vp9/common/vp9_common_data.c
    ${SOURCE_DIR}/vp9/common/x86/vp9_asm_stubs.c
    #${SOURCE_DIR}/vp9/common/x86/vp9_loopfilter_intrin_sse2.c
    ${SOURCE_DIR}/vp9/common/vp9_postproc.c
    #${SOURCE_DIR}/vp9/common/x86/vp9_idct_intrin_sse2.c
    ${SOURCE_DIR}/vp9/common/vp9_rtcd.c
    ${SOURCE_DIR}/vp9/vp9_cx_iface.c
    ${SOURCE_DIR}/vp9/vp9_dx_iface.c    
    ${SOURCE_DIR}/vp9/encoder/vp9_temporal_filter.c
    ${SOURCE_DIR}/vp9/encoder/vp9_temporal_filter.c
    ${SOURCE_DIR}/vp9/encoder/vp9_mcomp.c    
    ${SOURCE_DIR}/vp9/encoder/vp9_dct.c
    ${SOURCE_DIR}/vp9/encoder/vp9_sad_c.c
    ${SOURCE_DIR}/vp9/encoder/vp9_variance_c.c
    ${SOURCE_DIR}/vp9/encoder/vp9_encodemb.c
    ${SOURCE_DIR}/vp9/encoder/vp9_rdopt.c
    ${SOURCE_DIR}/vp9/encoder/vp9_lookahead.c
    ${SOURCE_DIR}/vp9/encoder/vp9_ratectrl.c
    ${SOURCE_DIR}/vp9/encoder/vp9_tokenize.c
    ${SOURCE_DIR}/vp9/encoder/vp9_quantize.c
    ${SOURCE_DIR}/vp9/encoder/vp9_encodemv.c
    ${SOURCE_DIR}/vp9/encoder/vp9_modecosts.c
    ${SOURCE_DIR}/vp9/encoder/vp9_treewriter.c
    ${SOURCE_DIR}/vp9/encoder/vp9_onyx_if.c
    ${SOURCE_DIR}/vp9/encoder/vp9_boolhuff.c
    ${SOURCE_DIR}/vp9/encoder/vp9_firstpass.c
    ${SOURCE_DIR}/vp9/encoder/vp9_psnr.c
    ${SOURCE_DIR}/vp9/encoder/vp9_bitstream.c
    ${SOURCE_DIR}/vp9/encoder/vp9_encodeframe.c
    ${SOURCE_DIR}/vp9/encoder/vp9_mbgraph.c
    ${SOURCE_DIR}/vp9/encoder/vp9_segmentation.c
    ${SOURCE_DIR}/vp9/encoder/vp9_picklpf.c
    ${SOURCE_DIR}/vp9/encoder/vp9_encodeintra.c
    ${SOURCE_DIR}/vp9/encoder/vp9_subexp.c
    ${SOURCE_DIR}/vp9/decoder/vp9_dboolhuff.c
    ${SOURCE_DIR}/vp9/decoder/vp9_decodemv.c
    ${SOURCE_DIR}/vp9/decoder/vp9_decodframe.c
    ${SOURCE_DIR}/vp9/decoder/vp9_detokenize.c
    ${SOURCE_DIR}/vp9/decoder/vp9_thread.c
    ${SOURCE_DIR}/vp9/decoder/vp9_onyxd_if.c
    ${SOURCE_DIR}/vp9/decoder/vp9_idct_blk.c
    ${SOURCE_DIR}/vp9/decoder/vp9_dsubexp.c
    #${SOURCE_DIR}/vp9/decoder/x86/vp9_dequantize_sse2.c
    ${SOURCE_DIR}/vpx_config.c
)

include_directories( ${SOURCE_DIR} )

add_library( ${PROJECT_NAME}_static STATIC ${sources} )
