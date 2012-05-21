set( MY_LIB_NAME MengeImageCodecPlugin )

ADD_FILTER(
src
	ImageCodecPlugin.h
	ImageCodecPlugin.cpp
	Decoder.h
	Decoder.cpp
	Encoder.h
	Encoder.cpp
	ImageDecoder.h
	ImageDecoder.cpp
	ImageEncoder.h
	ImageEncoder.cpp
	#VideoDecoder.h
	#VideoDecoder.cpp
	SoundDecoder.h
	SoundDecoder.cpp
	ImageCodecMNE.cpp
	ImageCodecMNE.h
	ImageDecoderJPEG.cpp
	ImageDecoderJPEG.h
	ImageDecoderMNE.cpp
	ImageDecoderMNE.h
	ImageDecoderPNG.cpp
	ImageDecoderPNG.h
    ImageDecoderDDS.cpp
    ImageDecoderDDS.h
	ImageEncoderJPEG.cpp
	ImageEncoderJPEG.h
	ImageEncoderPNG.cpp
	ImageEncoderPNG.h
	ImageDecoderCombinerRGBAndAlpha.cpp
    ImageDecoderCombinerRGBAndAlpha.h
	SoundDecoderOGGVorbis.cpp
	SoundDecoderOGGVorbis.h

#   ImageDecoderWEBP.h
#   ImageDecoderWEBP.cpp
)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils 
					 ${THIRDPARTY_DIR}
                     ${THIRDPARTY_DIR}/vorbis/include
					 ${THIRDPARTY_DIR}/ogg/include
					 #${THIRDPARTY_DIR}/theora/include
                     ${THIRDPARTY_DIR}/zlib
					 ${THIRDPARTY_DIR}/libpng
                     ${THIRDPARTY_DIR}/libjpeg
					
					 #${THIRDPARTY_DIR}/libWEBP/Include
					 #${THIRDPARTY_DIR}/libWEBPLL
)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    
    debug ${MENGINE_LIB_PREFIX}png_static${MENGINE_LIB_SUFFIXD}
    optimized ${MENGINE_LIB_PREFIX}png_static${MENGINE_LIB_SUFFIX}
    
    debug ${MENGINE_LIB_PREFIX}vorbisfile_static${MENGINE_LIB_SUFFIXD}
    optimized ${MENGINE_LIB_PREFIX}vorbisfile_static${MENGINE_LIB_SUFFIX}
    
    debug ${MENGINE_LIB_PREFIX}vorbis_static${MENGINE_LIB_SUFFIXD}
    optimized ${MENGINE_LIB_PREFIX}vorbis_static${MENGINE_LIB_SUFFIX}
    
    debug ${MENGINE_LIB_PREFIX}ogg_static${MENGINE_LIB_SUFFIXD}
    optimized ${MENGINE_LIB_PREFIX}ogg_static${MENGINE_LIB_SUFFIX}
    
    debug ${MENGINE_LIB_PREFIX}zlib_static${MENGINE_LIB_SUFFIXD}
    optimized ${MENGINE_LIB_PREFIX}zlib_static${MENGINE_LIB_SUFFIX}    
    
    debug ${MENGINE_LIB_PREFIX}jpeg_static${MENGINE_LIB_SUFFIXD}
    optimized ${MENGINE_LIB_PREFIX}jpeg_static${MENGINE_LIB_SUFFIX}

	#debug ${MENGINE_LIB_PREFIX}libwebp_a_debug${MENGINE_LIB_SUFFIX}
    #optimized ${MENGINE_LIB_PREFIX}libwebp_a${MENGINE_LIB_SUFFIX}

	#debug ${MENGINE_LIB_PREFIX}webpll_static${MENGINE_LIB_SUFFIX}
    #optimized ${MENGINE_LIB_PREFIX}webpll_static${MENGINE_LIB_SUFFIX}

    )

SET( APPLICATION_LIB_PATHS
 ${APPLICATION_LIB_PATHS}
 PARENT_SCOPE
)

SET( APPLICATION_LIBS
 ${APPLICATION_LIBS}
 PARENT_SCOPE
)

#SET( APPLICATION_LIB_PATHS
# ${APPLICATION_LIB_PATHS}
# ${THIRDPARTY_DIR}/libWEBP/Lib/
# ${THIRDPARTY_DIR}/libWEBPLL/
#)


