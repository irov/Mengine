set(MY_LIB_NAME SoundCodecPlugin)

ADD_FILTER(
src
	SoundCodecPlugin.cpp
	SoundCodecPlugin.def
	SoundCodecPlugin.h
    SoundDecoderWAV.cpp
    SoundDecoderWAV.h
)

ADD_MENGINE_LIBRARY(Plugins)

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} ${THIRDPARTY_LIB_DIR}/${MENGINE_LIB_PREFIX}stdex${MENGINE_LIB_SUFFIX})