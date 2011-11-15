
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
	VideoDecoder.h
	VideoDecoder.cpp
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
	SoundDecoderOGGVorbis.cpp
	SoundDecoderOGGVorbis.h
#	VideoDecoderOGGTheora.cpp
#	VideoDecoderOGGTheora.h
)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils 
					 ${THIRDPARTY_DIR}
                     ${THIRDPARTY_DIR}/vorbis/include
					 ${THIRDPARTY_DIR}/ogg/include
					 ${THIRDPARTY_DIR}/theora/include
                     ${THIRDPARTY_DIR}/libPNG
                     ${THIRDPARTY_DIR}/libJPEG

${THIRDPARTY_DIR}/zlib
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
 PARENT_SCOPE
)
