MENGINE_PROJECT(IOSAVSoundSystem)

ADD_FILTER(
    src
    IOSAVSoundSystem.h
    IOSAVSoundSystem.mm
    IOSAVSoundBuffer.h
    IOSAVSoundBuffer.mm
    IOSAVSoundSource.h
    IOSAVSoundSource.mm
)

ADD_MENGINE_LIBRARY(Systems)

IF(MENGINE_TARGET_IOS)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} "-framework AVFoundation" "-framework AudioToolbox" "-framework CoreAudio" )
ENDIF()
