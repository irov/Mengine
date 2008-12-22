
ADD_FILTER(
src
	ALSoundBuffer.cpp
	ALSoundBuffer.h
	ALSoundBufferStream.cpp
	ALSoundBufferStream.h
	ALSoundSource.cpp
	ALSoundSource.h
	SulkSystem.cpp
	SulkSystem.h
	VorbisCallback.cpp
	VorbisCallback.h
	ALSoundSystem.cpp
	ALSoundSystem.h
)

ADD_LIBRARY( OALSoundSystem ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 OALSoundSystem
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)