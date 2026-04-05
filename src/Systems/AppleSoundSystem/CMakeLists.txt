MENGINE_PROJECT(AppleSoundSystem)

ADD_FILTER(
src
    AppleSoundSystem.mm
    AppleSoundSystem.h
    AppleSoundSystemExtensionInterface.h

    AppleSoundBufferBase.cpp
    AppleSoundBufferBase.h
    AppleSoundBufferMemory.cpp
    AppleSoundBufferMemory.h
    AppleSoundBufferStream.cpp
    AppleSoundBufferStream.h
    AppleSoundSource.cpp
    AppleSoundSource.h
)

ADD_MENGINE_LIBRARY(Systems)

ADD_MENGINE_APPLE_FRAMEWORK(AudioToolbox)
ADD_MENGINE_APPLE_FRAMEWORK(CoreAudio)
ADD_MENGINE_APPLE_FRAMEWORK(AVFoundation)
