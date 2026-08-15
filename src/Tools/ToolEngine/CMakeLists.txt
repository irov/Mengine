MENGINE_PROJECT(ToolEngine)

ADD_FILTER(
src
    ToolEngine.h
    ToolEngine.cpp
)

ADD_MENGINE_LIBRARY()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC
    Kernel
    Engine
    Metacode
    ToolUtils
    DocumentService
    NotificationService
    EnumeratorService
    FactoryService
    CodecService
    ConverterService
    OptionsService
    FileService
    LoggerService
    PluginService
    ProviderService
    StringizeService
    ThreadService
    ArchiveService
    MemoryService
    TimelineService
    TimepipeService
    ModuleService
    PrototypeService
    VocabularyService
    ConfigService
    DataService
    ImageCodecPlugin
    ZipPlugin
    LZ4Plugin
    DevelopmentConverterPlugin
    MoviePlugin
    MetabufPlugin
    JPEGPlugin
    PNGPlugin
    PVRTCPlugin
    WebPPlugin
    TheoraPlugin
    OggVorbisPlugin
)

if(WIN32)
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC
        Win32AllocatorSystem
        Win32ThreadSystem
        Win32TimeSystem
        Win32DateTimeSystem
        Win32FileSystem
        Win32Platform
        Windows
    )
else()
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC
        POSIXAllocatorSystem
        POSIXThreadSystem
        POSIXDateTimeSystem
        POSIXTimeSystem
        AppleFileSystem
        MacOSPlatform
        Apple
        MacOS
        "-framework Security"
    )
endif()
