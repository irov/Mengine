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

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY(Services)