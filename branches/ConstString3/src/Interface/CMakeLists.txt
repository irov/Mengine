set( MY_LIB_NAME Interface )

if( MENGE_USE_MT )
	set( MY_LIB_NAME ${MY_LIB_NAME}_mt )
endif( MENGE_USE_MT )

ADD_FILTER(
src
	PluginInterface.h
	ScriptSystemInterface.h
	CodecInterface.h
	ImageCodecInterface.h
	SoundCodecInterface.h
	VideoCodecInterface.h
	ParticleSystemInterface.h
	FileSystemInterface.h
	RenderSystemInterface.h
	PhysicSystemInterface.h
	StreamInterface.h
	LogSystemInterface.h
	ApplicationInterface.h
	PhysicSystem2DInterface.h
	InputSystemInterface.h
	MengineInterface.h
	HardwareBufferInterface.h
	SoundSystemInterface.h
	TextureInterface.h
	ThreadSystemInterface.h	dummy.cpp


)

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )


SET( MENGE_DEPENDENCIES
 ${MENGE_DEPENDENCIES}
 ${MY_LIB_NAME}
)
 
SET( MENGE_DEPENDENCIES
 ${MENGE_DEPENDENCIES}
 PARENT_SCOPE
)

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
