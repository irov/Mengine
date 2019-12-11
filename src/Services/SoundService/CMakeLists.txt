SET(MY_LIB_NAME SoundService)

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