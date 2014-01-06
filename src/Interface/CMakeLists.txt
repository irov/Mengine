set( MY_LIB_NAME Interface )

ADD_FILTER(
src     
    ServiceInterface.h
	PluginInterface.h
    ModuleInterface.h
	ScriptSystemInterface.h
	CodecInterface.h
    DataInterface.h
    CacheInterface.h
    PlatformInterface.h
	XmlCodecInterface.h
	ImageCodecInterface.h
    PickCodecInterface.h
	SoundCodecInterface.h
	VideoCodecInterface.h
	ParticleSystemInterface.h
	FileSystemInterface.h
	RenderSystemInterface.h
	PhysicSystemInterface.h
    TextInterface.h
	StreamInterface.h
	LogSystemInterface.h
	ApplicationInterface.h
	PhysicSystem2DInterface.h
	InputSystemInterface.h
	MengineInterface.h
	SoundSystemInterface.h
	TextureInterface.h
	ThreadSystemInterface.h
	HTTPSystemInterface.h
	ConverterInterface.h
	StringizeInterface.h
    GameInterface.h
    NodeInterface.h
    PlayerInterface.h
    TimingManagerInterface.h
    ScheduleManagerInterface.h
    MousePickerSystemInterface.h
    MovieKeyFrameInterface.h
    PlayerInterface.h
    GameInterface.h    
    PrototypeManagerInterface.h
    WatchdogInterface.h
    EventInterface.h
    WindowsLayerInterface.h
    ArchiveInterface.h
    ProfilerInterface.h
    Model3DInterface.h    
    
	dummy.cpp
)
INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils )


ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

TARGET_LINK_LIBRARIES(${MY_LIB_NAME} Core)


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
