set( MY_LIB_NAME MengeImageCodecPlugin )

ADD_FILTER(
src
	ImageCodecPlugin.h
	ImageCodecPlugin.cpp
    
  
	ImageDecoderJPEG.cpp
	ImageDecoderJPEG.h
	ImageDecoderPNG.cpp
	ImageDecoderPNG.h
    
    ImageDecoderETC1.cpp
    ImageDecoderETC1.h
    ImageDecoderPVRTC.cpp
    ImageDecoderPVRTC.h

	ImageEncoderJPEG.cpp
	ImageEncoderJPEG.h    
	ImageEncoderPNG.cpp
	ImageEncoderPNG.h
       
    PickVerifyHIT.h
    
    PickDecoderHIT.cpp
    PickDecoderHIT.h
    
    PickEncoderHIT.cpp
    PickEncoderHIT.h
    
    ImageDecoderHTF.cpp
    ImageDecoderHTF.h       
    ImageEncoderHTF.cpp
    ImageEncoderHTF.h
    ImageVerifyHTF.h

    ImageDecoderACF.cpp
    ImageDecoderACF.h       
    ImageEncoderACF.cpp
    ImageEncoderACF.h
    ImageVerifyACF.h
    
    ImageCodecDDS.h
    ImageDecoderDDS.cpp
    ImageDecoderDDS.h    
    ImageEncoderDDS.cpp
    ImageEncoderDDS.h
    
    DataflowAEK.h
    DataflowAEK.cpp
    MovieFramePack.h
    MovieFramePack.cpp

    DataflowMDL.h
    DataflowMDL.cpp
    Model3DPack.h
    Model3DPack.cpp
)

SET(MENGINE_SUPPORT_DECODER_WEBP OFF)

if(MENGINE_SUPPORT_DECODER_WEBP)
    ADD_LIBRARY( ${MY_LIB_NAME} ImageDecoderWEBP.h )
    ADD_LIBRARY( ${MY_LIB_NAME} ImageDecoderWEBP.cpp )
    
    add_definitions(-DMENGINE_SUPPORT_DECODER_WEBP)
    INCLUDE_DIRECTORIES( ${THIRDPARTY_DIR}/libwebp/src )
    SET( APPLICATION_LIBS ${APPLICATION_LIBS} ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}webp_static${MENGINE_LIB_SUFFIX} )
endif()

INCLUDE_DIRECTORIES( 
        ${Mengine_SOURCE_DIR}/Utils 
        ${THIRDPARTY_DIR}
        #${THIRDPARTY_DIR}/theora/include
        ${THIRDPARTY_DIR}/zlib
        ${THIRDPARTY_DIR}/libpng
        ${THIRDPARTY_DIR}/libjpeg
        ${THIRDPARTY_DIR}/metabuf
					
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
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}png_static${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}vorbisfile_static${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}vorbis_static${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}ogg_static${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}zlib_static${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}jpeg_static${MENGINE_LIB_SUFFIX}
    ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}stdex_static${MENGINE_LIB_SUFFIX}    
    Logger
    Core
    Codec
    Archive
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


