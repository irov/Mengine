set( MY_LIB_NAME Interface )

ADD_FILTER(
src     
    ServiceInterface.h
    ServiceProviderInterface.h
	ServantInterface.h
	PluginInterface.h
    ModuleInterface.h
	ScriptSystemInterface.h
	CodecInterface.h
    ConfigInterface.h
    DataInterface.h
    MemoryInterface.h
    PlatformInterface.h
	XmlCodecInterface.h
	ImageCodecInterface.h
    PickCodecInterface.h
	SoundCodecInterface.h
	VideoCodecInterface.h
    TextInterface.h
	StreamInterface.h
	LoggerInterface.h
	ApplicationInterface.h
	InputSystemInterface.h	
	ThreadInterface.h
	ThreadSystemInterface.h
	HttpSystemInterface.h
	ConverterInterface.h
	StringizeInterface.h
    GameInterface.h
    NodeInterface.h
    PlayerInterface.h
    ScheduleManagerInterface.h
    MousePickerSystemInterface.h
    MovieKeyFrameInterface.h
    PlayerInterface.h
    GameInterface.h    
    PrototypeManagerInterface.h
    WatchdogInterface.h
    WindowsLayerInterface.h
    ArchiveInterface.h
    ArchiveServiceInterface.h
    ProfilerInterface.h
    PrefetcherInterface.h
    AmplifierInterface.h
    GraveyardInterface.h
    GlobalHandleSystemInterface.h
    NotificationServiceInterface.h
    SDLLayerInterface.h
    TimerInterface.h
    PackageInterface.h
    UserdataInterface.h    
    OptionsInterface.h
    TimelineInterface.h
    SocketInterface.h
	FactoryInterface.h
    ResourceServiceInterface.h
    UnicodeInterface.h
    NotificatorInterface.h
    LoaderInterface.h
    RandomizerInterface.h
    AnimationInterface.h
    AccountInterface.h
    EventationInterface.h
    UnknownInterface.h    
    RenderProgramVariableInterface.h
    RenderServiceInterface.h
    RenderInterface.h
    UpdateServiceInterface.h
    UpdationInterface.h
    
	dummy.cpp
)

ADD_FILTER(
Sound
    SoundSystemInterface.h
    SoundSourceInterface.h
    SoundServiceInterface.h
    SoundIdentityInterface.h
    SoundBufferInterface.h
    SoundVolumeProviderInterface.h
)

ADD_FILTER(
File
    FileSystemInterface.h
	FileInputStreamInterface.h
	FileMappedStreamInterface.h
	FileOutputStreamInterface.h
	FileGroupInterface.h
)   

ADD_FILTER(
Render     
    RenderEnumInterface.h
	RenderSystemInterface.h
	RenderImageInterface.h
	RenderTextureInterface.h
	RenderViewportInterface.h	
	RenderCameraInterface.h
    RenderTransformationInterface.h
	RenderScissorInterface.h
	RenderTargetInterface.h
	RenderMaterialServiceInterface.h
	RenderTextureServiceInterface.h
	RenderIndexBufferInterface.h
	RenderVertexBufferInterface.h
	RenderProgramInterface.h
	RenderVertexShaderInterface.h
	RenderFragmentShaderInterface.h
	RenderVertexAttributeInterface.h
	RenderMaterialInterface.h   
)

ADD_FILTER(
dummy     
	dummy.cpp
)

INCLUDE_DIRECTORIES( ${Mengine_SOURCE_DIR}/Utils )

ADD_LIBRARY( ${MY_LIB_NAME} ${SRC_FILES} )

SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 ${MY_LIB_NAME}
)


SET( APPLICATION_DEPENDENCIES
 ${APPLICATION_DEPENDENCIES}
 PARENT_SCOPE
)
