MENGINE_PROJECT(ImageCodecPlugin)

ADD_FILTER(
src
    ImageCodecPlugin.h
    ImageCodecPlugin.cpp
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

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_IMAGECODEC)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)