MENGINE_PROJECT(SoundService)

ADD_FILTER(
src
    SoundIdentity.h
    SoundIdentity.cpp
    SoundService.h
    SoundService.cpp
    ThreadWorkerSoundBufferUpdate.h
    ThreadWorkerSoundBufferUpdate.cpp
)

ADD_MENGINE_LIBRARY(Services)