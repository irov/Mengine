set( MY_LIB_NAME ImageCodecPlugin )

ADD_FILTER(
src
	ImageCodecPlugin.h
	ImageCodecPlugin.cpp
)

ADD_FILTER(
jpeg  
	ImageDecoderJPEG.cpp
	ImageDecoderJPEG.h
	ImageEncoderJPEG.cpp
	ImageEncoderJPEG.h    
)

ADD_FILTER(
png
	ImageDecoderPNG.cpp
	ImageDecoderPNG.h
	ImageEncoderPNG.cpp
	ImageEncoderPNG.h
)
    
ADD_FILTER(
etc1    
    ImageDecoderETC1.cpp
    ImageDecoderETC1.h
)

ADD_FILTER(
pvrtc
    ImageDecoderPVRTC.cpp
    ImageDecoderPVRTC.h
)

ADD_FILTER(
hit    
    PickVerifyHIT.h
    
    PickDecoderHIT.cpp
    PickDecoderHIT.h
    
    PickEncoderHIT.cpp
    PickEncoderHIT.h
)

ADD_FILTER(
htf    
    ImageDecoderHTF.cpp
    ImageDecoderHTF.h       
    ImageEncoderHTF.cpp
    ImageEncoderHTF.h
    ImageVerifyHTF.h
)

ADD_FILTER(
acf
    ImageDecoderACF.cpp
    ImageDecoderACF.h       
    ImageEncoderACF.cpp
    ImageEncoderACF.h
    ImageVerifyACF.h
)

ADD_FILTER(
dds    
    ImageCodecDDS.h
    ImageDecoderDDS.cpp
    ImageDecoderDDS.h    
    ImageEncoderDDS.cpp
    ImageEncoderDDS.h
)

ADD_FILTER(
aek    
    DataflowAEK.h
    DataflowAEK.cpp
    MovieFramePack.h
    MovieFramePack.cpp
)

ADD_FILTER(
mdl
    DataflowMDL.h
    DataflowMDL.cpp
    Model3DPack.h
    Model3DPack.cpp
)

SET(MENGINE_SUPPORT_DECODER_WEBP ON)

if(MENGINE_SUPPORT_DECODER_WEBP)
    ADD_FILTER(
        webp
        ImageDecoderWEBP.h
        ImageDecoderWEBP.cpp 
    )
    
    add_definitions(-DMENGINE_SUPPORT_DECODER_WEBP)
    INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/libwebp/src )
endif()

INCLUDE_DIRECTORIES( 
        ${Mengine_SOURCE_DIR}/Utils 
        ${THIRDPARTY_DIR}
        #${THIRDPARTY_DIR}/theora/include
        ${THIRDPARTY_DIR}/zlib
        ${THIRDPARTY_DIR}/libpng
        ${THIRDPARTY_DIR}/libjpeg
        ${THIRDPARTY_DIR}/libmath/include
        ${THIRDPARTY_DIR}/metabuf
					
        #${THIRDPARTY_DIR}/libWEBP/Include
        #${THIRDPARTY_DIR}/libWEBPLL
)

ADD_MENGINE_LIBRARY()

set_target_properties (${MY_LIB_NAME} PROPERTIES
    FOLDER Plugins
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)

if(MENGINE_SUPPORT_DECODER_WEBP)
    SET( APPLICATION_LIBS 
		${APPLICATION_LIBS} 
		${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}webp${MENGINE_LIB_SUFFIX} 
		)    
endif()

if(MSVC)
    SET( APPLICATION_LIBS 
		${APPLICATION_LIBS} 
		${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}png_static${MENGINE_LIB_SUFFIX} 
		)    
else()
    SET( APPLICATION_LIBS 
		${APPLICATION_LIBS} 
		${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}png${MENGINE_LIB_SUFFIX} 
		)    
endif()
    
SET( APPLICATION_LIBS
    ${APPLICATION_LIBS}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}vorbisfile${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}vorbis${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}ogg${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zlib${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}jpeg${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX}    
    Kernel
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


