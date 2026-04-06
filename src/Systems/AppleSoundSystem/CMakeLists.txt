MENGINE_PROJECT(AppleSoundSystem)

ADD_FILTER(
src
    AppleSoundSystem.mm
    AppleSoundSystem.h
    AppleSoundSystemExtensionInterface.h

    AppleSoundBufferBase.mm
    AppleSoundBufferBase.h
    AppleSoundBufferMemory.mm
    AppleSoundBufferMemory.h
    AppleSoundPCMHelper.mm
    AppleSoundPCMHelper.h
    AppleSoundBufferStream.mm
    AppleSoundBufferStream.h
    AppleSoundSource.mm
    AppleSoundSource.h
)

ADD_MENGINE_LIBRARY(Systems)

ADD_MENGINE_APPLE_FRAMEWORK(AudioToolbox)
ADD_MENGINE_APPLE_FRAMEWORK(CoreAudio)
ADD_MENGINE_APPLE_FRAMEWORK(AVFoundation)
