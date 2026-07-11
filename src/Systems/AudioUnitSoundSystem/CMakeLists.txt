MENGINE_PROJECT(AudioUnitSoundSystem)

ADD_FILTER(
src
    AudioUnitSoundSystem.mm
    AudioUnitSoundSystem.h
    AudioUnitSoundSystemExtensionInterface.h

    AudioUnitSoundBufferBase.mm
    AudioUnitSoundBufferBase.h
    AudioUnitSoundBufferMemory.mm
    AudioUnitSoundBufferMemory.h
    AudioUnitSoundPCMHelper.mm
    AudioUnitSoundPCMHelper.h
    AudioUnitSoundBufferStream.mm
    AudioUnitSoundBufferStream.h
    AudioUnitSoundSource.mm
    AudioUnitSoundSource.h
)

ADD_MENGINE_LIBRARY(Systems)

ADD_MENGINE_APPLE_FRAMEWORK(AudioToolbox)
ADD_MENGINE_APPLE_FRAMEWORK(CoreAudio)
ADD_MENGINE_APPLE_FRAMEWORK(AVFoundation)
