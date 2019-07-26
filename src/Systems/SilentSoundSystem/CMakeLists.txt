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

INCLUDE_DIRECTORIES(${THIRDPARTY_DIR}/libmath/include)

ADD_MENGINE_LIBRARY(Systems)