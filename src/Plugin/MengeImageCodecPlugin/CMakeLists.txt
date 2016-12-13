set( MY_LIB_NAME MengeImageCodecPlugin )

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

if(MENGINE_SUPPORT_DECODER_WEBP)
    SET( APPLICATION_LIBS ${APPLICATION_LIBS} ${THIRDPARTY_DIR}/${MENGINE_LIB_DIR}/${CMAKE_BUILD_TYPE}/${MENGINE_LIB_PREFIX}webp${MENGINE_LIB_SUFFIX} )
    
endif()
    
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


