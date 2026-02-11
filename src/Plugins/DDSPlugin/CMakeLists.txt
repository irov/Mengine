MENGINE_PROJECT(DDSPlugin)

ADD_FILTER(
src
    DDSPlugin.h
    DDSPlugin.def
    DDSPlugin.cpp

    ImageCodecDDS.h
    ImageDecoderDDS.cpp
    ImageDecoderDDS.h
    ImageEncoderDDS.cpp
    ImageEncoderDDS.h
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_DDS)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} Kernel)