set( MY_LIB_NAME Interface )

ADD_FILTER(
src     
    ServiceInterface.h
	PluginInterface.h
    ModuleInterface.h
	ScriptSystemInterface.h
	CodecInterface.h
    ConfigInterface.h
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
	InputSystemInterface.h
	SoundSystemInterface.h
	ThreadSystemInterface.h
	HttpSystemInterface.h
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
    PrefetcherInterface.h
    AmplifierInterface.h
    GraveyardInterface.h
    GlobalHandleSystemInterface.h
    NotificationServiceInterface.h
    MarmaladeLayerInterface.h
    SDLLayerInterface.h
    
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
