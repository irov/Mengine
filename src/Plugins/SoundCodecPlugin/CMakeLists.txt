MENGINE_PROJECT(SoundCodecPlugin)

ADD_FILTER(
src
    SoundCodecPlugin.cpp
    SoundCodecPlugin.def
    SoundCodecPlugin.h
    SoundDecoderWAV.cpp
    SoundDecoderWAV.h
)

ADD_MENGINE_PLUGIN(MENGINE_PLUGIN_SOUNDCODEC)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})