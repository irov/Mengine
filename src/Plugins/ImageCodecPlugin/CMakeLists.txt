MENGINE_PROJECT(ImageCodecPlugin)

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

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/zlib)
INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/zlib)

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libpng)
INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libjpeg)
INCLUDE_DIRECTORIES(${THIRDPARTY_CONFIG_DIR}/jpeg)


ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IMAGECODEC)

if(MENGINE_TOOLCHAIN_MSVC)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}libpng16_static${MENGINE_DEPENDENCIES_DEBUG_SUFFIX}${MENGINE_LIB_SUFFIX})
else()
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}png16${MENGINE_DEPENDENCIES_DEBUG_SUFFIX}${MENGINE_LIB_SUFFIX})
endif()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}ogg${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}zlib${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}jpeg${MENGINE_LIB_SUFFIX})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)