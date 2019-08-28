set(MY_LIB_NAME SilentSoundSystem)

ADD_FILTER(
src
	SilentSoundBuffer.cpp
	SilentSoundBuffer.h
	SilentSoundSource.cpp
	SilentSoundSource.h
	SilentSoundSystem.cpp
	SilentSoundSystem.h
)

ADD_MENGINE_LIBRARY(Systems)