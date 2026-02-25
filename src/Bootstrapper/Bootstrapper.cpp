#include "Bootstrapper.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/ModuleServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/AccountServiceInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/GameServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/AnalyticsServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/RenderServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/VectorConstString.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/Stringalized.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/VectorString.h"
#include "Kernel/Stringstream.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/FileLogger.h"
#include "Kernel/BuildMode.h"
#include "Kernel/FilePathDateTimeHelper.h"
#include "Kernel/DefaultPrototypeGenerator.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/TimestampHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/Crash.h"
#include "Kernel/FileContent.h"
#include "Kernel/EntityEventable.h"
#include "Kernel/MT19937Randomizer.h"
#include "Kernel/PityDice.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/MixerMultiplicative.h"
#include "Kernel/MixerAveraging.h"
#include "Kernel/MixerBoolean.h"
#include "Kernel/StatisticHelper.h"

#include "Config/Path.h"

#include "Configuration/Configurations.h"
#include "Configuration/SecureValue.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_BOOTSTRAPPER_LOAD_CONFIG
#define MENGINE_BOOTSTRAPPER_LOAD_CONFIG 1
#endif
//////////////////////////////////////////////////////////////////////////
#if MENGINE_BOOTSTRAPPER_LOAD_CONFIG == 1
#   define MENGINE_BOOTSTRAPPER_LOAD_CONFIG_ENABLE
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_BOOTSTRAPPER_CONFIG_PATH
#define MENGINE_BOOTSTRAPPER_CONFIG_PATH "application.json"
#endif
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( LifecycleService );
SERVICE_EXTERN( SecureService );
SERVICE_EXTERN( FactoryService );
SERVICE_EXTERN( OptionsService );
SERVICE_EXTERN( LoggerService );
SERVICE_EXTERN( ArrowService );
SERVICE_EXTERN( PlatformService );
SERVICE_EXTERN( NotificationService );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( ConfigService );
SERVICE_EXTERN( SettingsService );
SERVICE_EXTERN( ArchiveService );
SERVICE_EXTERN( AnalyticsService );
SERVICE_EXTERN( ThreadService );
SERVICE_EXTERN( SoundService );
SERVICE_EXTERN( ModuleService );
SERVICE_EXTERN( CodecService );
SERVICE_EXTERN( DataService );
SERVICE_EXTERN( PrefetcherService );
SERVICE_EXTERN( MemoryService );
#if defined(MENGINE_MASTER_RELEASE_DISABLE)
SERVICE_EXTERN( ConverterService );
#endif
SERVICE_EXTERN( InputService );
SERVICE_EXTERN( PluginService );
SERVICE_EXTERN( EasingService );
SERVICE_EXTERN( PrototypeService );
SERVICE_EXTERN( VocabularyService );
SERVICE_EXTERN( UpdateService );
SERVICE_EXTERN( LoaderService );
SERVICE_EXTERN( RenderService );
SERVICE_EXTERN( RenderMaterialService );
SERVICE_EXTERN( RenderTextureService );
SERVICE_EXTERN( ResourceService );
SERVICE_EXTERN( SceneService );
SERVICE_EXTERN( TextService );
SERVICE_EXTERN( FontService );
SERVICE_EXTERN( WatchdogService );
SERVICE_EXTERN( GraveyardService );
SERVICE_EXTERN( PackageService );
SERVICE_EXTERN( UserdataService );
SERVICE_EXTERN( PickerService );
SERVICE_EXTERN( PlayerService );
SERVICE_EXTERN( AccountService );
SERVICE_EXTERN( HttpService );
SERVICE_EXTERN( GameService );
SERVICE_EXTERN( TimerService );
SERVICE_EXTERN( TimelineService );
SERVICE_EXTERN( TimepipeService );
SERVICE_EXTERN( StatisticService );
SERVICE_EXTERN( Application );
SERVICE_EXTERN( ChronometerService );
SERVICE_EXTERN( AmplifierService );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( MockupRenderSystem );
SERVICE_EXTERN( SilentSoundSystem );
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_SYSTEM_FILE)
SERVICE_EXTERN( FileSystem );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_SYSTEM_SOUND)
SERVICE_EXTERN( SoundSystem );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_SYSTEM_TIME)
SERVICE_EXTERN( TimeSystem );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_SYSTEM_RENDER)
SERVICE_EXTERN( RenderSystem );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_SYSTEM_THREAD)
SERVICE_EXTERN( ThreadSystem );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_SYSTEM_CRYPTOGRAPHY)
SERVICE_EXTERN( CryptographySystem );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_SYSTEM_DATETIME)
SERVICE_EXTERN( DateTimeSystem );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_SYSTEM_HTTP)
SERVICE_EXTERN( HttpSystem );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM)
SERVICE_EXTERN( PlatformSystem );
#endif
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( EnvironmentService );
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_WINDOWS)
SERVICE_EXTERN( Win32KernelService );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_ANDROID)
SERVICE_EXTERN( AndroidKernelService );
SERVICE_EXTERN( AndroidAssetService );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_VENDOR_APPLE)
SERVICE_EXTERN( AppleKernelService );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_IOS)
SERVICE_EXTERN( iOSKernelService );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_EXTERNAL_BOOTSTRAPPER)
PLUGIN_EXPORT( ExternalBootstrapper );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_ZIP_STATIC)
PLUGIN_EXPORT( Zip );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_LZ4_STATIC)
PLUGIN_EXPORT( LZ4 );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_INI_STATIC)
PLUGIN_EXPORT( INI );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_IMAGE_CODEC_STATIC)
PLUGIN_EXPORT( ImageCodec );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_WAV_STATIC)
PLUGIN_EXPORT( WAV );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_OGG_VORBIS_STATIC)
PLUGIN_EXPORT( OggVorbis );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_ASTRALAX_STATIC)
PLUGIN_EXPORT( Astralax );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_DAZZLE_STATIC)
PLUGIN_EXPORT( Dazzle );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_BITMAPFONT_STATIC)
PLUGIN_EXPORT( BitmapFont );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_NODEDEBUGRENDER_STATIC)
PLUGIN_EXPORT( NodeDebugRender );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_DEBUGPANEL_STATIC)
PLUGIN_EXPORT( DebugPanel );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_LAYOUTEDITOR_STATIC)
PLUGIN_EXPORT( LayoutEditor );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_RESOURCEPREFETCHER_STATIC)
PLUGIN_EXPORT( ResourcePrefetcher );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_RESOURCECONVERT_STATIC)
PLUGIN_EXPORT( ResourceConvert );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_RESOURCEVALIDATE_STATIC)
PLUGIN_EXPORT( ResourceValidate );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_RESOURCEDEBUGGER_STATIC)
PLUGIN_EXPORT( ResourceDebugger );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_METABUF_STATIC)
PLUGIN_EXPORT( Metabuf );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_PVRTC_STATIC)
PLUGIN_EXPORT( PVRTC );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_ETC1_STATIC)
PLUGIN_EXPORT( ETC1 );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_DDS_STATIC)
PLUGIN_EXPORT( DDS );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_JPEG_STATIC)
PLUGIN_EXPORT( JPEG );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_PNG_STATIC)
PLUGIN_EXPORT( PNG );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_WEBP_STATIC)
PLUGIN_EXPORT( WebP );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_VIDEO_STATIC)
PLUGIN_EXPORT( Video );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_THEORA_STATIC)
PLUGIN_EXPORT( Theora );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_SPINE_STATIC)
PLUGIN_EXPORT( Spine );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_MOVIE_STATIC)
PLUGIN_EXPORT( Movie );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_TTF_STATIC)
PLUGIN_EXPORT( TTF );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_FE_STATIC)
PLUGIN_EXPORT( FE );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_WIN32_SOCKET_STATIC)
PLUGIN_EXPORT( Win32Socket );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_SDL2_SOCKET_STATIC)
PLUGIN_EXPORT( SDL2Socket );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_NODEDEBUGGER_STATIC)
PLUGIN_EXPORT( NodeDebugger );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_OZZ_ANIMATION_STATIC)
PLUGIN_EXPORT( OzzAnimation );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_GOAPFRAMEWORK_STATIC)
PLUGIN_EXPORT( GOAPFramework );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_PYTHONFRAMEWORK_STATIC)
PLUGIN_EXPORT( PythonFramework );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_SIMPLEFRAMEWORK_STATIC)
PLUGIN_EXPORT( SimpleFramework );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_UIFRAMEWORK_STATIC)
PLUGIN_EXPORT( UIFramework );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_IMGUIFRAMEWORK_STATIC)
PLUGIN_EXPORT( ImGUIFramework );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_JSON_STATIC)
PLUGIN_EXPORT( JSON );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_BOX2D_STATIC)
PLUGIN_EXPORT( Box2D );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_TEXTUREPACKER_STATIC)
PLUGIN_EXPORT( Texturepacker );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_GRAPHICS_STATIC)
PLUGIN_EXPORT( Graphics );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_WIN32_SENTRY_STATIC)
PLUGIN_EXPORT( Win32Sentry );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_STEAM_STATIC)
PLUGIN_EXPORT( Steam );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_GOAP_STATIC)
PLUGIN_EXPORT( GOAP );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_DEVTODEBUG_STATIC)
PLUGIN_EXPORT( DevToDebug );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_CACHALOT_STATIC)
PLUGIN_EXPORT( Cachalot );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_CAMERADEBUGGIZMO_STATIC)
PLUGIN_EXPORT( CameraDebugGizmo );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_FILEMODIFYHOOK_STATIC)
PLUGIN_EXPORT( FileModifyHook );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_MEMORYUSAGEMONITOR_STATIC)
PLUGIN_EXPORT( MemoryUsageMonitor );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_OPTICK_STATIC)
PLUGIN_EXPORT( Optick );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_WIN32_ANTIFREEZEMONITOR_STATIC)
PLUGIN_EXPORT( Win32AntifreezeMonitor );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_ANDROID_NATIVE_PYTHON_STATIC)
PLUGIN_EXPORT( AndroidNativePython );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_XMLTOBIN_STATIC)
PLUGIN_EXPORT( XmlToBin );
#endif
/////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Bootstrapper, Mengine::Bootstrapper );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Bootstrapper::Bootstrapper()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Bootstrapper::~Bootstrapper()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Bootstrapper::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::initialize()
    {
        LOGGER_INFO( "bootstrapper", "bootstrapper create services" );

        if( this->createServices_() == false )
        {
            LOGGER_ERROR( "invalid create create services" );

            return false;
        }

        LOGGER_INFO( "bootstrapper", "bootstrapper create dynamic priority plugins" );

        if( this->createDynamicSystemPlugins_() == false )
        {
            LOGGER_ERROR( "invalid create dynamic priority plugins" );

            return false;
        }

        LOGGER_INFO( "bootstrapper", "bootstrapper create dynamic priority dev plugins" );

        if( this->createDynamicSystemDevPlugins_() == false )
        {
            LOGGER_ASSERTION( "invalid create dynamic priority dev plugins" );

            return false;
        }

        LOGGER_INFO( "bootstrapper", "bootstrapper create sentry" );

        if( this->createSentryPlugins_() == false )
        {
            LOGGER_ERROR( "invalid create sentry" );

            return false;
        }

        LOGGER_INFO( "bootstrapper", "bootstrapper create static frameworks" );

        if( this->createStaticFrameworks_() == false )
        {
            LOGGER_ERROR( "invalid create static frameworks" );

            return false;
        }

        LOGGER_INFO( "bootstrapper", "bootstrapper create dynamic priority plugins" );

        if( this->createDynamicPriorityPlugins_() == false )
        {
            LOGGER_ERROR( "invalid create dynamic priority plugins" );

            return false;
        }

        LOGGER_INFO( "bootstrapper", "bootstrapper create dynamic priority dev plugins" );

        if( this->createDynamicPriorityDevPlugins_() == false )
        {
            LOGGER_ERROR( "invalid create dynamic priority dev plugins" );

            return false;
        }

        LOGGER_INFO( "bootstrapper", "bootstrapper create dynamic options plugins" );

        if( this->createDynamicOptionsPlugins_() == false )
        {
            LOGGER_ERROR( "invalid create options plugins" );

            return false;
        }

        LOGGER_INFO( "bootstrapper", "bootstrapper create dynamic static plugins" );

        if( this->createStaticPlugins_() == false )
        {
            LOGGER_ERROR( "invalid create static plugins" );

            return false;
        }

        LOGGER_INFO( "bootstrapper", "bootstrapper create dynamic plugins" );

        if( this->createDynamicPlugins_() == false )
        {
            LOGGER_ERROR( "invalid create dynamic plugins" );

            return false;
        }

        LOGGER_INFO( "bootstrapper", "bootstrapper create dynamic dev plugins" );

        if( this->createDynamicDevPlugins_() == false )
        {
            LOGGER_ERROR( "invalid create dynamic dev plugins" );

            return false;
        }

        LOGGER_INFO( "bootstrapper", "bootstrapper create application" );

        if( this->createApplication_() == false )
        {
            LOGGER_ERROR( "invalid create application" );

            return false;
        }

        LOGGER_INFO( "bootstrapper", "bootstrapper run modules" );

        if( this->runModules_() == false )
        {
            LOGGER_ERROR( "invalid run modules" );

            return false;
        }

        LOGGER_INFO( "bootstrapper", "bootstrapper run dev modules" );

        if( this->runDevModules_() == false )
        {
            LOGGER_ERROR( "invalid run dev modules" );

            return false;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_COMPLETE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::run()
    {
        THREAD_SERVICE()
            ->updateMainThread();

        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_RUN );

        LOGGER_INFO( "bootstrapper", "bootstrapper initialize game" );

        FileGroupInterfacePtr defaultFileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );

        if( APPLICATION_SERVICE()
            ->initializeGame( defaultFileGroup, m_packagesPaths ) == false )
        {
            LOGGER_FATAL( "application invalid initialize game" );

            return false;
        }

        LOGGER_INFO( "bootstrapper", "bootstrapper load personality" );

        if( GAME_SERVICE()
            ->loadPersonality() == false )
        {
            LOGGER_FATAL( "game invalid load personality" );

            return false;
        }

        LOGGER_INFO( "bootstrapper", "bootstrapper run" );

        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_RUN_COMPLETE );

        if( HAS_OPTION( "crash" ) == true )
        {
            Helper::crash( "Bootstrapper test crash" );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static ConfigInterfacePtr loadApplicationConfig( const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc )
        {
            if( _content->exist( true ) == false )
            {
                return nullptr;
            }

            ConfigInterfacePtr applicationConfig = Helper::loadConfig( _content, ConstString::none(), _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( applicationConfig, "invalid open application settings '%s'"
                , Helper::getContentFullPath( _content ).c_str()
            );

            return applicationConfig;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::loadApplicationConfig_()
    {
        FileGroupInterfacePtr defaultFileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), ConstString::none() );

        FilePath applicationConfigPath = Helper::stringizeFilePath( MENGINE_BOOTSTRAPPER_CONFIG_PATH );

        ContentInterfacePtr applicationConfigContent = Helper::makeFileContent( defaultFileGroup, applicationConfigPath, MENGINE_DOCUMENT_FACTORABLE );

        ConfigInterfacePtr applicationConfig = Detail::loadApplicationConfig( applicationConfigContent, MENGINE_DOCUMENT_FACTORABLE );

        if( applicationConfig == nullptr )
        {
            LOGGER_ERROR( "invalid load application config '%s'"
                , applicationConfigPath.c_str()
            );

            return false;
        }

        VectorFilePath configsPaths;
        applicationConfig->getValues( "Configs", "Path", &configsPaths );

        for( const FilePath & filePath : configsPaths )
        {
            ContentInterfacePtr content = Helper::makeFileContent( defaultFileGroup, filePath, MENGINE_DOCUMENT_FACTORABLE );

            ConfigInterfacePtr config = Helper::loadConfig( content, ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

            if( config == nullptr )
            {
                LOGGER_ERROR( "invalid load include config '%s'"
                    , filePath.c_str()
                );

                return false;
            }

            CONFIG_SERVICE()
                ->addBackConfig( config );
        }

        VectorFilePath credentialsPaths;
        applicationConfig->getValues( "Credentials", "Path", &credentialsPaths );

        for( const FilePath & filePath : credentialsPaths )
        {
            ContentInterfacePtr content = Helper::makeFileContent( defaultFileGroup, filePath, MENGINE_DOCUMENT_FACTORABLE );

            ConfigInterfacePtr config = Helper::loadConfig( content, ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

            if( config == nullptr )
            {
                LOGGER_ERROR( "invalid load credential '%s'"
                    , filePath.c_str()
                );

                return false;
            }

            CONFIG_SERVICE()
                ->addBackConfig( config );
        }

        applicationConfig->getValues( "Packages", "Path", &m_packagesPaths );

        const Char * option_config = GET_OPTION_VALUE( "config", nullptr );

        if( option_config != nullptr )
        {
            ContentInterfacePtr content = Helper::makeFileContent( defaultFileGroup, Helper::stringizeFilePath( option_config ), MENGINE_DOCUMENT_FACTORABLE );

            ConfigInterfacePtr config = Helper::loadConfig( content, ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

            if( config == nullptr )
            {
                LOGGER_ERROR( "invalid load option config '%s'"
                    , option_config
                );

                return false;
            }

            CONFIG_SERVICE()
                ->addBackConfig( config );
        }

        VectorString options;
        CONFIG_VALUES( "Engine", "Option", &options );

        for( const String & option : options )
        {
            OPTIONS_SERVICE()
                ->setOptionValue( option.c_str(), "" );
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_CONFIGS_LOAD );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::mountUserFileGroup_()
    {
        Path userPath = {'\0'};
        size_t userPathLen = PLATFORM_SERVICE()
            ->getUserPath( userPath );

        FilePath cs_userPath = Helper::stringizeFilePathSize( userPath, (FilePath::size_type)userPathLen );

        bool OPTION_showuserfolder = HAS_OPTION( "showuserfolder" );

        if( OPTION_showuserfolder == true )
        {
            PLATFORM_SERVICE()
                ->messageBox( "Mengine", "user folder '%s'", cs_userPath.c_str() );
        }
        else
        {
            LOGGER_INFO_PROTECTED( "bootstrapper", "user folder '%s'", cs_userPath.c_str() );
        }

        FileGroupInterfacePtr userFileGroup = nullptr;

        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "user" ), nullptr, nullptr, cs_userPath, STRINGIZE_STRING_LOCAL( "global" ), &userFileGroup, true, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "failed to mount user directory '%s'"
                , userPath
            );

            return false;
        }

        VOCABULARY_SET( FileGroupInterface, STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "user" ), userFileGroup, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_NOTIFY( NOTIFICATOR_MOUNT_USER_FILEGROUP );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::initializeFileLogger_()
    {
        bool OPTION_nologs = HAS_OPTION( "nologs" );

        if( OPTION_nologs == true )
        {
            return true;
        }

        Path filePathDate = {'\0'};
        Helper::makeFilePathDateTimestamp( filePathDate, MENGINE_MAX_PATH );

        WString unicode_date;
        Helper::utf8ToUnicode( filePathDate, &unicode_date );

        WString unicode_logFilename;
        unicode_logFilename += L"Game";

        bool developmentMode = Helper::isDevelopmentMode();
        bool OPTION_roaming = HAS_OPTION( "roaming" );
        bool OPTION_noroaming = HAS_OPTION( "noroaming" );

        bool developmentLog = false;

        if( developmentMode == true && (OPTION_roaming == false || OPTION_noroaming == false) )
        {
            developmentLog = true;
        }

        if( developmentLog == true )
        {
            unicode_logFilename += L"_";
            unicode_logFilename += unicode_date;
        }

        unicode_logFilename += L".log";
        String utf8_logFilename;
        if( Helper::unicodeToUtf8( unicode_logFilename, &utf8_logFilename ) == false )
        {
            LOGGER_ASSERTION( "failed log directory '%ls' convert to ut8f"
                , unicode_logFilename.c_str()
            );

            return false;
        }

        FilePath logFilename = Helper::stringizeFilePath( utf8_logFilename );

        const FileGroupInterfacePtr & userFileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        if( developmentLog == false )
        {
            ContentInterfacePtr fileOldLogContent = Helper::makeFileContent( userFileGroup, logFilename, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( fileOldLogContent, "invalid make file content '%s'"
                , logFilename.c_str()
            );

            LOGGER_SERVICE()
                ->loadOldLogMemory( fileOldLogContent );
        }

        FileLoggerPtr fileLog = Helper::makeFactorableUnique<FileLogger>( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( fileLog, "invalid create file logger" );

        ContentInterfacePtr fileLogContent = Helper::makeFileContent( userFileGroup, logFilename, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( fileLogContent, "invalid make file content '%s'"
            , logFilename.c_str()
        );

        fileLog->setContent( fileLogContent );

        fileLog->setWriteHistory( true );

        if( LOGGER_SERVICE()
            ->registerLogger( fileLog ) == false )
        {
            LOGGER_ASSERTION( "invalid register file logger '%s'"
                , logFilename.c_str()
            );
        }
        else
        {
            m_loggerFile = fileLog;

            Path fullLogFilename = {'\0'};
            userFileGroup->getFullPath( logFilename, fullLogFilename );

            LOGGER_INFO_PROTECTED( "bootstrapper", "write file log to '%s'"
                , fullLogFilename
            );

            LOGGER_SERVICE()
                ->setCurrentContentLog( fileLogContent );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::registerBaseTypes_()
    {
        LOGGER_INFO( "bootstrapper", "register base generator..." );

        if( PROTOTYPE_SERVICE()
            ->addPrototype( ConstString::none(), EntityEventable::getFactorableType(), Helper::makeDefaultPrototypeGenerator<EntityEventable, 128>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Randomizer" ), MT19937Randomizer::getFactorableType(), Helper::makeDefaultPrototypeGenerator<MT19937Randomizer, 8>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Dice" ), PityDice::getFactorableType(), Helper::makeDefaultPrototypeGenerator<PityDice, 32>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( ConstString::none(), MixerMultiplicative::getFactorableType(), Helper::makeDefaultPrototypeGenerator<MixerMultiplicative, 32>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( ConstString::none(), MixerAveraging::getFactorableType(), Helper::makeDefaultPrototypeGenerator<MixerAveraging, 32>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        if( PROTOTYPE_SERVICE()
            ->addPrototype( ConstString::none(), MixerBoolean::getFactorableType(), Helper::makeDefaultPrototypeGenerator<MixerBoolean, 32>( MENGINE_DOCUMENT_FACTORABLE ) ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Bootstrapper::unregisterBaseTypes_()
    {
        LOGGER_INFO( "bootstrapper", "unregister base generator..." );

        PROTOTYPE_SERVICE()
            ->removePrototype( ConstString::none(), EntityEventable::getFactorableType(), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Randomizer" ), MT19937Randomizer::getFactorableType(), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Dice" ), PityDice::getFactorableType(), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( ConstString::none(), MixerMultiplicative::getFactorableType(), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( ConstString::none(), MixerAveraging::getFactorableType(), nullptr );

        PROTOTYPE_SERVICE()
            ->removePrototype( ConstString::none(), MixerBoolean::getFactorableType(), nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_ADD_FRAMEWORK( Name, Info, Doc )\
        {\
            if(SERVICE_IS_INITIALIZE(LoggerServiceInterface) == true)\
            {\
                LOGGER_INFO( "bootstrapper", "%s", Info );\
            }\
            if( PLUGIN_CREATE(Name, true, Doc) == false )\
            {\
                if(SERVICE_IS_INITIALIZE(LoggerServiceInterface) == true)\
                {\
                    LOGGER_ERROR( "%s [invalid initialize]", Info );\
                }\
                return false;\
            }\
        }
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_ADD_PLUGIN( Name, Info, Doc )\
        {\
            if(SERVICE_IS_INITIALIZE(LoggerServiceInterface) == true)\
            {\
                LOGGER_INFO( "bootstrapper", "%s", Info );\
            }\
            if( PLUGIN_CREATE(Name, false, Doc) == false )\
            {\
                if(SERVICE_IS_INITIALIZE(LoggerServiceInterface) == true)\
                {\
                    LOGGER_ERROR( "%s [invalid initialize]", Info );\
                }\
                return false;\
            }\
        }
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_ADD_SERVICE( Name, Doc )\
        {\
            if(SERVICE_IS_INITIALIZE(LoggerServiceInterface) == true)\
            {\
                LOGGER_INFO( "bootstrapper", "create service: " #Name);\
            }\
            if( SERVICE_CREATE(Name, Doc) == false )\
            {\
                if(SERVICE_IS_INITIALIZE(LoggerServiceInterface) == true)\
                {\
                    LOGGER_ERROR("invalid create service: " #Name);\
                }\
                return false;\
            }\
        }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createServices_()
    {
        MENGINE_ADD_SERVICE( PlatformSystem, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( ThreadSystem, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( TimeSystem, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( LifecycleService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( NotificationService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( VocabularyService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( DateTimeSystem, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( CryptographySystem, MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_PLATFORM_WINDOWS)
        MENGINE_ADD_SERVICE( Win32KernelService, MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLATFORM_ANDROID)
        MENGINE_ADD_SERVICE( AndroidKernelService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( AndroidAssetService, MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_VENDOR_APPLE)
        MENGINE_ADD_SERVICE( AppleKernelService, MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLATFORM_IOS)
        MENGINE_ADD_SERVICE( iOSKernelService, MENGINE_DOCUMENT_FACTORABLE );
#endif

        MENGINE_ADD_SERVICE( OptionsService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( FactoryService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( PrototypeService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( MemoryService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( LoggerService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( SecureService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( AnalyticsService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( StatisticService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( TimerService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( TimelineService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( TimepipeService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( PluginService, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ADD_SERVICE( EnvironmentService, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ADD_SERVICE( PlatformService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( FileService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( ConfigService, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ADD_SERVICE( FileSystem, MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_DEBUG)
        LOGGER_MESSAGE( "secure: %s", MENGINE_SECURE_VALUE );
#endif

        LOGGER_INFO( "bootstrapper", "debug mode '%s'", Helper::isDebugMode() == true ? "ON" : "OFF" );
        LOGGER_INFO( "bootstrapper", "development mode '%s'", Helper::isDevelopmentMode() == true ? "ON" : "OFF" );
        LOGGER_INFO( "bootstrapper", "build publish '%s'", Helper::isBuildPublish() == true ? "ON" : "OFF" );
        LOGGER_INFO( "bootstrapper", "master release '%s'", Helper::isMasterRelease() == true ? "ON" : "OFF" );

#if defined(MENGINE_ASSERTION_DEBUG_ENABLE)
        LOGGER_INFO( "bootstrapper", "enable assertion debug: [ON]" );
#else
        LOGGER_INFO( "bootstrapper", "enable assertion debug: [OFF]" );
#endif

#if defined(MENGINE_ALLOCATOR_DEBUG_ENABLE)
        LOGGER_INFO( "bootstrapper", "enable allocator debug: [ON]" );
#else
        LOGGER_INFO( "bootstrapper", "enable allocator debug: [OFF]" );
#endif

#if defined(MENGINE_DOCUMENT_ENABLE)
        LOGGER_INFO( "bootstrapper", "enable document debug: [ON]" );
#else
        LOGGER_INFO( "bootstrapper", "enable document debug: [OFF]" );
#endif

        if( this->registerBaseTypes_() == false )
        {
            return false;
        }

#if defined(MENGINE_DEBUG)
        Path currentPath = {'\0'};
        PLATFORM_SERVICE()
            ->getCurrentPath( currentPath );

        LOGGER_INFO_PROTECTED( "bootstrapper", "current folder '%s'"
            , currentPath
        );
#endif

#if defined(MENGINE_PLUGIN_INI_STATIC)
        MENGINE_ADD_PLUGIN( INI, "plugin INI...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_JSON_STATIC)
        MENGINE_ADD_PLUGIN( JSON, "plugin JSON...", MENGINE_DOCUMENT_FACTORABLE );
#endif

        PLATFORM_SERVICE()
            ->initializeFileService();

#if defined(MENGINE_BOOTSTRAPPER_LOAD_CONFIG_ENABLE)
        LOGGER_INFO( "bootstrapper", "bootstrapper load application config" );

        if( this->loadApplicationConfig_() == false )
        {
            return false;
        }
#endif

        bool OPTION_assertion = HAS_OPTION( "assertion" );
        bool Engine_AssertionDebugBreak = CONFIG_VALUE_BOOLEAN( "Engine", "AssertionDebugBreak", false );

        bool Assertion_NoDebugBreak = OPTION_assertion == false && Engine_AssertionDebugBreak == false;

        MENGINE_ASSERTION_SET_NOT_DEBUG_BREAK( Assertion_NoDebugBreak );

        LOGGER_INFO( "bootstrapper", "bootstrapper mount user file group" );

        if( this->mountUserFileGroup_() == false )
        {
            LOGGER_ERROR( "invalid mount user file group" );

            return false;
        }

#if defined(MENGINE_PLUGIN_OPTICK_STATIC)
        MENGINE_ADD_PLUGIN( Optick, "initialize Optick...", MENGINE_DOCUMENT_FACTORABLE );
#endif

        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_BASE_SERVICES );

        LOGGER_INFO( "bootstrapper", "bootstrapper initialize file logger" );

        if( this->initializeFileLogger_() == false )
        {
            LOGGER_ASSERTION( "invalid initalize file logger" );

            return false;
        }

#define BOOTSTRAPPER_SERVICE_CREATE( Name, Doc )\
        LOGGER_INFO( "bootstrapper", "bootstrapper create service: " #Name );\
        if( SERVICE_CREATE( Name, Doc ) == false)\
        {\
            LOGGER_ERROR( "invalid initialize service: " #Name );\
            return false;\
        }

        BOOTSTRAPPER_SERVICE_CREATE( SettingsService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( ThreadService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( ArchiveService, MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_PLUGIN_ZIP_STATIC)
        MENGINE_ADD_PLUGIN( Zip, "plugin Zip...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_LZ4_STATIC)
        MENGINE_ADD_PLUGIN( LZ4, "plugin LZ4...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_SYSTEM_RENDER)
        bool OPTION_norender = HAS_OPTION( "norender" );

        if( OPTION_norender == true )
        {
            BOOTSTRAPPER_SERVICE_CREATE( MockupRenderSystem, MENGINE_DOCUMENT_FACTORABLE );
        }
        else
        {
            BOOTSTRAPPER_SERVICE_CREATE( RenderSystem, MENGINE_DOCUMENT_FACTORABLE );
        }
#endif

        BOOTSTRAPPER_SERVICE_CREATE( RenderService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( RenderMaterialService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( RenderTextureService, MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_SYSTEM_SOUND)
        bool OPTION_mute = HAS_OPTION( "mute" );

        if( OPTION_mute == true )
        {
            BOOTSTRAPPER_SERVICE_CREATE( SilentSoundSystem, MENGINE_DOCUMENT_FACTORABLE );
        }
        else
        {
            if( SERVICE_CREATE_SAFE( SoundSystem, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                BOOTSTRAPPER_SERVICE_CREATE( SilentSoundSystem, MENGINE_DOCUMENT_FACTORABLE );
            }
        }
#endif

#if defined(MENGINE_SYSTEM_HTTP)
        BOOTSTRAPPER_SERVICE_CREATE( HttpSystem, MENGINE_DOCUMENT_FACTORABLE );
#endif

        BOOTSTRAPPER_SERVICE_CREATE( HttpService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( SoundService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( ModuleService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( CodecService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( DataService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( PrefetcherService, MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        BOOTSTRAPPER_SERVICE_CREATE( ConverterService, MENGINE_DOCUMENT_FACTORABLE );
#endif

        BOOTSTRAPPER_SERVICE_CREATE( InputService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( ArrowService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( ChronometerService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( EasingService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( UpdateService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( LoaderService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( SceneService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( ResourceService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( TextService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( FontService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( WatchdogService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( GraveyardService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( PackageService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( UserdataService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( PickerService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( PlayerService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( AccountService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( GameService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( AmplifierService, MENGINE_DOCUMENT_FACTORABLE );

#undef BOOTSTRAPPER_SERVICE_CREATE

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createSentryPlugins_()
    {
        LOGGER_INFO( "bootstrapper", "create sentry plugins..." );

#if defined(MENGINE_PLUGIN_WIN32_SENTRY_STATIC)
        MENGINE_ADD_PLUGIN( Win32Sentry, "plugin Win32Sentry...", MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createStaticFrameworks_()
    {
        LOGGER_INFO( "bootstrapper", "create static frameworks..." );

#if defined(MENGINE_PLUGIN_GOAPFRAMEWORK_STATIC)
        MENGINE_ADD_FRAMEWORK( GOAPFramework, "plugin GOAPFramework...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_IMGUIFRAMEWORK_STATIC)
        MENGINE_ADD_FRAMEWORK( ImGUIFramework, "plugin ImGUIFramework...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_PYTHONFRAMEWORK_STATIC)
        MENGINE_ADD_FRAMEWORK( PythonFramework, "plugin PythonFramework...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_SIMPLEFRAMEWORK_STATIC)
        MENGINE_ADD_FRAMEWORK( SimpleFramework, "plugin SimpleFramework...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_UIFRAMEWORK_STATIC)
        MENGINE_ADD_FRAMEWORK( UIFramework, "plugin UIFramework...", MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createStaticPlugins_()
    {
        LOGGER_INFO( "bootstrapper", "create plugins..." );

#if defined(MENGINE_EXTERNAL_BOOTSTRAPPER)
        MENGINE_ADD_PLUGIN( ExternalBootstrapper, "initialize external Bootstrapper [" MENGINE_PP_STRINGIZE( MENGINE_EXTERNAL_BOOTSTRAPPER ) "]...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_NODEDEBUGRENDER_STATIC)
        MENGINE_ADD_PLUGIN( NodeDebugRender, "plugin NodeDebugRender...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_DEBUGPANEL_STATIC)
        MENGINE_ADD_PLUGIN( DebugPanel, "plugin DebugPanel...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_LAYOUTEDITOR_STATIC)
        MENGINE_ADD_PLUGIN( LayoutEditor, "plugin LayoutEditor...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_RESOURCEPREFETCHER_STATIC)
        MENGINE_ADD_PLUGIN( ResourcePrefetcher, "plugin ResourcePrefetcher...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_RESOURCECONVERT_STATIC)
        MENGINE_ADD_PLUGIN( ResourceConvert, "plugin ResourceConvert...", MENGINE_DOCUMENT_FACTORABLE );
#endif        

#if defined(MENGINE_PLUGIN_RESOURCEVALIDATE_STATIC)
        MENGINE_ADD_PLUGIN( ResourceValidate, "plugin ResourceValidate...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_RESOURCEDEBUGGER_STATIC)
        MENGINE_ADD_PLUGIN( ResourceDebugger, "plugin ResourceDebugger...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_NODEDEBUGGER_STATIC)
        MENGINE_ADD_PLUGIN( NodeDebugger, "plugin NodeDebugger...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_OZZ_ANIMATION_STATIC)
        MENGINE_ADD_PLUGIN( OzzAnimation, "plugin OzzAnimation...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_BOX2D_STATIC)
        MENGINE_ADD_PLUGIN( Box2D, "plugin Box2D...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_TEXTUREPACKER_STATIC)
        MENGINE_ADD_PLUGIN( Texturepacker, "plugin Texturepacker...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_GRAPHICS_STATIC)
        MENGINE_ADD_PLUGIN( Graphics, "plugin Graphics...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_METABUF_STATIC)
        MENGINE_ADD_PLUGIN( Metabuf, "plugin Metabuf...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_IMAGE_CODEC_STATIC)
        MENGINE_ADD_PLUGIN( ImageCodec, "plugin ImageCodec...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_WAV_STATIC)
        MENGINE_ADD_PLUGIN( WAV, "plugin WAV...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_OGG_VORBIS_STATIC)
        MENGINE_ADD_PLUGIN( OggVorbis, "plugin OggVorbis...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_PVRTC_STATIC)
        MENGINE_ADD_PLUGIN( PVRTC, "plugin PVRTC...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_ETC1_STATIC)
        MENGINE_ADD_PLUGIN( ETC1, "plugin ETC1...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_DDS_STATIC)
        MENGINE_ADD_PLUGIN( DDS, "plugin DDS...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_JPEG_STATIC)
        MENGINE_ADD_PLUGIN( JPEG, "plugin JPEG...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_PNG_STATIC)
        MENGINE_ADD_PLUGIN( PNG, "plugin PNG...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_WEBP_STATIC)
        MENGINE_ADD_PLUGIN( WebP, "plugin WebP...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_VIDEO_STATIC)
        MENGINE_ADD_PLUGIN( Video, "plugin Video...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_THEORA_STATIC)
        MENGINE_ADD_PLUGIN( Theora, "plugin Theora...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_SPINE_STATIC)
        MENGINE_ADD_PLUGIN( Spine, "plugin Spine...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_MOVIE_STATIC)
        MENGINE_ADD_PLUGIN( Movie, "plugin Movie...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_ASTRALAX_STATIC)
        MENGINE_ADD_PLUGIN( Astralax, "plugin Astralax...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_DAZZLE_STATIC)
        MENGINE_ADD_PLUGIN( Dazzle, "plugin Dazzle...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_BITMAPFONT_STATIC)
        MENGINE_ADD_PLUGIN( BitmapFont, "plugin BitmapFont...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_TTF_STATIC)
        MENGINE_ADD_PLUGIN( TTF, "plugin TTF...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_FE_STATIC)
        MENGINE_ADD_PLUGIN( FE, "plugin FE...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_STEAM_STATIC)
        MENGINE_ADD_PLUGIN( Steam, "plugin Steam...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_WIN32_SOCKET_STATIC)
        MENGINE_ADD_PLUGIN( Win32Socket, "plugin Win32Socket...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_SDL2_SOCKET_STATIC)
        MENGINE_ADD_PLUGIN( SDL2Socket, "plugin SDL2Socket...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_GOAP_STATIC)
        MENGINE_ADD_PLUGIN( GOAP, "plugin GOAP...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_DEVTODEBUG_STATIC)
        MENGINE_ADD_PLUGIN( DevToDebug, "plugin DevToDebug...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_CACHALOT_STATIC)
        MENGINE_ADD_PLUGIN( Cachalot, "plugin Cachalot...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_CAMERADEBUGGIZMO_STATIC)
        MENGINE_ADD_PLUGIN( CameraDebugGizmo, "plugin CameraDebugGizmo...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_FILEMODIFYHOOK_STATIC)
        MENGINE_ADD_PLUGIN( FileModifyHook, "plugin FileModifyHook...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_MEMORYUSAGEMONITOR_STATIC)
        MENGINE_ADD_PLUGIN( MemoryUsageMonitor, "plugin MemoryUsageMonitor...", MENGINE_DOCUMENT_FACTORABLE );
#endif


#if defined(MENGINE_PLUGIN_WIN32_ANTIFREEZEMONITOR_STATIC)
        MENGINE_ADD_PLUGIN( Win32AntifreezeMonitor, "plugin Win32AntifreezeMonitor...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_ANDROID_NATIVE_PYTHON_STATIC)
        MENGINE_ADD_PLUGIN( AndroidNativePython, "plugin AndroidNativePython...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLUGIN_XMLTOBIN_STATIC)
        MENGINE_ADD_PLUGIN( XmlToBin, "plugin XmlToBin...", MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
#undef MENGINE_ADD_PLUGIN
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createDynamicPlugins_()
    {
        VectorString plugins;
        CONFIG_VALUES( "Plugins", "Name", &plugins );

        for( const String & pluginName : plugins )
        {
            LOGGER_INFO( "bootstrapper", "create dynamic plugin '%s'"
                , pluginName.c_str()
            );

            if( PLUGIN_SERVICE()
                ->loadPlugin( pluginName.c_str(), false, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_FATAL( "failed to load dynamic plugin '%s'"
                    , pluginName.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createDynamicDevPlugins_()
    {
        bool devplugins_avalilable = MENGINE_MASTER_RELEASE_VALUE( false, Helper::isDevelopmentMode() );

        if( HAS_OPTION( "devplugins" ) == true )
        {
            devplugins_avalilable = true;
        }

        if( HAS_OPTION( "nodevplugins" ) == true )
        {
            devplugins_avalilable = false;
        }

        if( devplugins_avalilable == false )
        {
            return true;
        }

        VectorString devPlugins;
        CONFIG_VALUES( "DevPlugins", "Name", &devPlugins );

        for( const String & pluginName : devPlugins )
        {
            LOGGER_INFO( "bootstrapper", "create dynamic dev plugin '%s'"
                , pluginName.c_str()
            );

            if( PLUGIN_SERVICE()
                ->loadPlugin( pluginName.c_str(), false, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_WARNING( "failed to load dynamic dev plugin '%s'"
                    , pluginName.c_str()
                );
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createDynamicSystemPlugins_()
    {
        VectorString plugins;
        CONFIG_VALUES( "SystemPlugins", "Name", &plugins );

        for( const String & pluginName : plugins )
        {
            LOGGER_INFO( "bootstrapper", "create dynamic system plugin '%s'"
                , pluginName.c_str()
            );

            if( PLUGIN_SERVICE()
                ->loadPlugin( pluginName.c_str(), false, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "failed to load dynamic system plugin '%s'"
                    , pluginName.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createDynamicSystemDevPlugins_()
    {
        bool devplugins = MENGINE_MASTER_RELEASE_VALUE( false, Helper::isDevelopmentMode() );

        if( HAS_OPTION( "devplugins" ) == true )
        {
            devplugins = true;
        }

        if( HAS_OPTION( "nodevplugins" ) == true )
        {
            devplugins = false;
        }

        if( devplugins == false )
        {
            return true;
        }

        VectorString devPlugins;
        CONFIG_VALUES( "SystemDevPlugins", "Name", &devPlugins );

        for( const String & pluginName : devPlugins )
        {
            LOGGER_INFO( "bootstrapper", "create dynamic system dev plugin '%s'"
                , pluginName.c_str()
            );

            if( PLUGIN_SERVICE()
                ->loadPlugin( pluginName.c_str(), false, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_WARNING( "failed to load dynamic system dev plugin '%s'"
                    , pluginName.c_str()
                );
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createDynamicPriorityPlugins_()
    {
        VectorString plugins;
        CONFIG_VALUES( "PriorityPlugins", "Name", &plugins );

        for( const String & pluginName : plugins )
        {
            LOGGER_INFO( "bootstrapper", "create dynamic priority plugin '%s'"
                , pluginName.c_str()
            );

            if( PLUGIN_SERVICE()
                ->loadPlugin( pluginName.c_str(), false, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "failed to load priority plugin '%s'"
                    , pluginName.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createDynamicPriorityDevPlugins_()
    {
        bool devplugins = MENGINE_MASTER_RELEASE_VALUE( false, Helper::isDevelopmentMode() );

        if( HAS_OPTION( "devplugins" ) == true )
        {
            devplugins = true;
        }

        if( HAS_OPTION( "nodevplugins" ) == true )
        {
            devplugins = false;
        }

        if( devplugins == false )
        {
            return true;
        }

        VectorString devPlugins;
        CONFIG_VALUES( "PriorityDevPlugins", "Name", &devPlugins );

        for( const String & pluginName : devPlugins )
        {
            LOGGER_INFO( "bootstrapper", "create dynamic priority dev plugin '%s'"
                , pluginName.c_str()
            );

            if( PLUGIN_SERVICE()
                ->loadPlugin( pluginName.c_str(), false, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_WARNING( "failed to load priority dev plugin '%s'"
                    , pluginName.c_str()
                );
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createDynamicOptionsPlugins_()
    {
        const Char * optionsPlugins[MENGINE_OPTIONS_VALUES_MAX] = {nullptr};
        uint32_t optionsPluginsCount;
        if( OPTIONS_SERVICE()
            ->getOptionValues( "plugins", optionsPlugins, &optionsPluginsCount ) == false )
        {
            return true;
        }

        for( const Char
            ** it_plugin = optionsPlugins,
            **it_plugin_end = optionsPlugins + optionsPluginsCount;
            it_plugin != it_plugin_end; ++it_plugin )
        {
            const Char * pluginName = *it_plugin;

            LOGGER_INFO( "bootstrapper", "create dynamic options plugin '%s'"
                , pluginName
            );

            if( PLUGIN_SERVICE()
                ->loadPlugin( pluginName, false, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "failed to load dynamic options plugin '%s'"
                    , pluginName
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createApplication_()
    {
        LOGGER_INFO( "bootstrapper", "create application..." );

        Timestamp mengine_create_application_timestamp = ANALYTICS_SERVICE()
            ->buildEvent( AEEC_SYSTEM, STRINGIZE_STRING_LOCAL( "mng_create_application_start" ), MENGINE_DOCUMENT_FACTORABLE )
            ->log();

        if( SERVICE_CREATE_SAFE( Application, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );

        ANALYTICS_SERVICE()
            ->buildEvent( AEEC_SYSTEM, STRINGIZE_STRING_LOCAL( "mng_create_application_completed" ), MENGINE_DOCUMENT_FACTORABLE )
            ->addParameterInteger( STRINGIZE_STRING_LOCAL( "time" ), Helper::getSystemDurationTimestamp( mengine_create_application_timestamp ) )
            ->log();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::runModules_()
    {
        LOGGER_INFO( "bootstrapper", "create modules..." );

        VectorConstString modules;
        CONFIG_VALUES( "Modules", "Name", &modules );

        for( const ConstString & moduleName : modules )
        {
            LOGGER_INFO( "bootstrapper", "run module '%s'"
                , moduleName.c_str()
            );

            if( MODULE_SERVICE()
                ->runModule( moduleName, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "failed to run module '%s'"
                    , moduleName.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::runDevModules_()
    {
#if defined(MENGINE_MASTER_RELEASE_ENABLE)
        bool devmodules = false;
#else
#   if defined(MENGINE_DEBUG)
        bool devmodules = true;
#   else
        bool developmentMode = Helper::isDevelopmentMode();
        bool devmodules = developmentMode;
#   endif
#endif

        if( HAS_OPTION( "devmodules" ) == true )
        {
            devmodules = true;
        }

        bool OPTION_nodevmodules = HAS_OPTION( "nodevmodules" );

        if( devmodules == true && OPTION_nodevmodules == false )
        {
            VectorConstString devModules;
            CONFIG_VALUES( "DevModules", "Name", &devModules );

            for( const ConstString & moduleName : devModules )
            {
                if( MODULE_SERVICE()
                    ->hasModule( moduleName ) == false )
                {
                    LOGGER_ASSERTION( "not exist dev module '%s'"
                        , moduleName.c_str()
                    );

                    continue;
                }

                LOGGER_INFO( "bootstrapper", "run dev module '%s'"
                    , moduleName.c_str()
                );

                if( MODULE_SERVICE()
                    ->runModule( moduleName, MENGINE_DOCUMENT_FACTORABLE ) == false )
                {
                    LOGGER_ASSERTION( "failed to run dev module '%s'"
                        , moduleName.c_str()
                    );
                }
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Bootstrapper::stopModules_()
    {
        if( SERVICE_IS_INITIALIZE( ModuleServiceInterface ) == false )
        {
            return;
        }

        VectorConstString modules;
        CONFIG_VALUES( "Modules", "Name", &modules );

        for( const ConstString & moduleName : modules )
        {
            if( MODULE_SERVICE()
                ->hasModule( moduleName ) == false )
            {
                LOGGER_ERROR( "not exist module '%s'"
                    , moduleName.c_str()
                );

                continue;
            }

            if( MODULE_SERVICE()
                ->isRunModule( moduleName ) == false )
            {
                LOGGER_ERROR( "not run module '%s'"
                    , moduleName.c_str()
                );

                continue;
            }

            LOGGER_INFO( "bootstrapper", "stop module '%s'"
                , moduleName.c_str()
            );

            if( MODULE_SERVICE()
                ->stopModule( moduleName ) == false )
            {
                LOGGER_ERROR( "failed to stop module '%s'"
                    , moduleName.c_str()
                );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Bootstrapper::stopDevModules_()
    {
        if( SERVICE_IS_INITIALIZE( ModuleServiceInterface ) == false )
        {
            return;
        }

#if defined(MENGINE_MASTER_RELEASE_ENABLE)
        bool devmodules = false;
#else
#   if defined(MENGINE_DEBUG)
        bool devmodules = true;
#   else
        bool developmentMode = Helper::isDevelopmentMode();
        bool devmodules = developmentMode;
#   endif
#endif

        if( HAS_OPTION( "devmodules" ) == true )
        {
            devmodules = true;
        }

        bool OPTION_nodevmodules = HAS_OPTION( "nodevmodules" );

        if( devmodules == true && OPTION_nodevmodules == false )
        {
            VectorConstString devModules;
            CONFIG_VALUES( "DevModules", "Name", &devModules );

            for( const ConstString & moduleName : devModules )
            {
                if( MODULE_SERVICE()
                    ->hasModule( moduleName ) == false )
                {
                    LOGGER_ASSERTION( "not exist dev module '%s'"
                        , moduleName.c_str()
                    );

                    continue;
                }

                if( MODULE_SERVICE()
                    ->isRunModule( moduleName ) == false )
                {
                    LOGGER_ASSERTION( "not run dev module '%s'"
                        , moduleName.c_str()
                    );

                    continue;
                }

                LOGGER_INFO( "bootstrapper", "stop dev module '%s'"
                    , moduleName.c_str()
                );

                if( MODULE_SERVICE()
                    ->stopModule( moduleName ) == false )
                {
                    LOGGER_ASSERTION( "failed to stop dev module '%s'"
                        , moduleName.c_str()
                    );
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Bootstrapper::stop()
    {
        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_ENGINE_STOP );
        }

        if( SERVICE_IS_INITIALIZE( GameServiceInterface ) == true )
        {
            GAME_SERVICE()
                ->interruption();
        }

        SERVICE_FINALIZE( SceneService );

        if( SERVICE_IS_INITIALIZE( GameServiceInterface ) == true )
        {
            GAME_SERVICE()
                ->stop();
        }

        m_packagesPaths.clear();

        if( SERVICE_IS_INITIALIZE( ApplicationInterface ) == true )
        {
            APPLICATION_SERVICE()
                ->finalizeGame();
        }

        if( SERVICE_IS_INITIALIZE( AccountServiceInterface ) == true )
        {
            ACCOUNT_SERVICE()
                ->stopAccounts();
        }

        if( SERVICE_IS_INITIALIZE( GameServiceInterface ) == true )
        {
            GAME_SERVICE()
                ->removePersonality();
        }

        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_ENGINE_PREPARE_FINALIZE );
            NOTIFICATION_NOTIFY( NOTIFICATOR_ENGINE_FINALIZE );
        }

        this->stopModules_();
        this->stopDevModules_();

        SERVICE_PROVIDER_STOP();

        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_ENGINE_STOP_SERVICES );
        }  

        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_ENGINE_STOP_THREADS );
        }

        if( SERVICE_IS_INITIALIZE( ThreadServiceInterface ) == true )
        {
            THREAD_SERVICE()
                ->stopTasks();
        }

        if( SERVICE_IS_INITIALIZE( RenderServiceInterface ) == true )
        {
            RENDER_SERVICE()
                ->destroyRenderWindow();
        }

        SERVICE_FINALIZE( HttpService );
        SERVICE_FINALIZE( GameService );
        SERVICE_FINALIZE( ArrowService );
        SERVICE_FINALIZE( Application );

        SERVICE_FINALIZE( SecureService );
        SERVICE_FINALIZE( AccountService );
        SERVICE_FINALIZE( ChronometerService );
        SERVICE_FINALIZE( PrefetcherService );
        SERVICE_FINALIZE( PackageService );
        SERVICE_FINALIZE( UserdataService );
        SERVICE_FINALIZE( GraveyardService );
        SERVICE_FINALIZE( ResourceService );
        SERVICE_FINALIZE( TextService );
        SERVICE_FINALIZE( FontService );
        SERVICE_FINALIZE( SettingsService );
        SERVICE_FINALIZE( DataService );
        SERVICE_FINALIZE( ConfigService );

        if( SERVICE_IS_INITIALIZE( PluginServiceInterface ) == true )
        {
            PLUGIN_SERVICE()
                ->unloadPlugins();
        }

        SERVICE_FINALIZE( LoaderService );
        SERVICE_FINALIZE( TimerService );
        SERVICE_FINALIZE( TimelineService );
        SERVICE_FINALIZE( WatchdogService );
        SERVICE_FINALIZE( ModuleService );
        SERVICE_FINALIZE( PlayerService );
        SERVICE_FINALIZE( AmplifierService );
        SERVICE_FINALIZE( PickerService );
        SERVICE_FINALIZE( UpdateService );
        SERVICE_FINALIZE( InputService );
        SERVICE_FINALIZE( CodecService );
        SERVICE_FINALIZE( SoundService );

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        SERVICE_FINALIZE( ConverterService );
#endif

        SERVICE_FINALIZE( RenderService );
        SERVICE_FINALIZE( RenderMaterialService );
        SERVICE_FINALIZE( RenderTextureService );
        SERVICE_FINALIZE( ArchiveService );
        SERVICE_FINALIZE( ThreadService );
        SERVICE_FINALIZE( EasingService );

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ) );

        SERVICE_FINALIZE( PluginService );
        SERVICE_FINALIZE( TimepipeService );

        LOGGER_SERVICE()
            ->setCurrentContentLog( nullptr );


        if( m_loggerFile != nullptr )
        {
            LOGGER_MESSAGE( "finish log success" );

            LOGGER_SERVICE()
                ->unregisterLogger( m_loggerFile );

            m_loggerFile = nullptr;
        }

        PLATFORM_SERVICE()
            ->finalizeFileService();

        if( SERVICE_IS_INITIALIZE( FileServiceInterface ) == true )
        {
            FILE_SERVICE()
                ->unmountFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );
        }

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "user" ) );

        SERVICE_FINALIZE( FileService );
        SERVICE_FINALIZE( RenderSystem );
        SERVICE_FINALIZE( SoundSystem );
        SERVICE_FINALIZE( FileSystem );

#if defined(MENGINE_SYSTEM_HTTP)
        SERVICE_FINALIZE( HttpSystem );
#endif

#if defined(MENGINE_PLATFORM_WINDOWS)
        SERVICE_FINALIZE( Win32KernelService );
#endif

#if defined(MENGINE_PLATFORM_ANDROID)
        SERVICE_FINALIZE( AndroidAssetService );
        SERVICE_FINALIZE( AndroidKernelService );
#endif

#if defined(MENGINE_VENDOR_APPLE)
        SERVICE_FINALIZE( AppleKernelService );
#endif

#if defined(MENGINE_PLATFORM_IOS)
        SERVICE_FINALIZE( iOSKernelService );
#endif

        SERVICE_FINALIZE( PlatformService );

        SERVICE_FINALIZE( EnvironmentService );

        this->unregisterBaseTypes_();

        SERVICE_FINALIZE( AnalyticsService );
        SERVICE_FINALIZE( StatisticService );

        SERVICE_FINALIZE( LoggerService );
        SERVICE_FINALIZE( MemoryService );
        SERVICE_FINALIZE( OptionsService );
        SERVICE_FINALIZE( VocabularyService );
        SERVICE_FINALIZE( NotificationService );
        SERVICE_FINALIZE( PrototypeService );
        SERVICE_FINALIZE( FactoryService );
        SERVICE_FINALIZE( DateTimeSystem );
        SERVICE_FINALIZE( ThreadSystem );
        SERVICE_FINALIZE( TimeSystem );
        SERVICE_FINALIZE( CryptographySystem );
        SERVICE_FINALIZE( PlatformSystem );


#if defined(MENGINE_SYSTEM_HTTP)
        SERVICE_DESTROY( HttpSystem );
#endif

        SERVICE_DESTROY( FileSystem );

        SERVICE_DESTROY( HttpService );
        SERVICE_DESTROY( SceneService );
        SERVICE_DESTROY( GameService );
        SERVICE_DESTROY( AmplifierService );
        SERVICE_DESTROY( Application );
        SERVICE_DESTROY( SecureService );
        SERVICE_DESTROY( AccountService );
        SERVICE_DESTROY( ChronometerService );
        SERVICE_DESTROY( PrefetcherService );
        SERVICE_DESTROY( PackageService );
        SERVICE_DESTROY( UserdataService );
        SERVICE_DESTROY( GraveyardService );
        SERVICE_DESTROY( ResourceService );
        SERVICE_DESTROY( TextService );
        SERVICE_DESTROY( FontService );
        SERVICE_DESTROY( DataService );
        SERVICE_DESTROY( LoaderService );
        SERVICE_DESTROY( TimerService );
        SERVICE_DESTROY( TimelineService );
        SERVICE_DESTROY( WatchdogService );
        SERVICE_DESTROY( ModuleService );
        SERVICE_DESTROY( PlayerService );
        SERVICE_DESTROY( PickerService );
        SERVICE_DESTROY( UpdateService );
        SERVICE_DESTROY( InputService );
        SERVICE_DESTROY( CodecService );
        SERVICE_DESTROY( SoundService );
        SERVICE_DESTROY( SoundSystem );

#if defined(MENGINE_MASTER_RELEASE_DISABLE)
        SERVICE_DESTROY( ConverterService );
#endif

        SERVICE_DESTROY( RenderService );
        SERVICE_DESTROY( PrototypeService );
        SERVICE_DESTROY( RenderMaterialService );
        SERVICE_DESTROY( RenderTextureService );
        SERVICE_DESTROY( RenderSystem );
        SERVICE_DESTROY( ConfigService );
        SERVICE_DESTROY( SettingsService );
        SERVICE_DESTROY( ArchiveService );
        SERVICE_DESTROY( MemoryService );
        SERVICE_DESTROY( ThreadService );
        SERVICE_DESTROY( DateTimeSystem );
        SERVICE_DESTROY( EasingService );
        SERVICE_DESTROY( VocabularyService );
        SERVICE_DESTROY( PluginService );
        SERVICE_DESTROY( ArrowService );
        SERVICE_DESTROY( FileService );
        SERVICE_DESTROY( TimeSystem );
        SERVICE_DESTROY( TimepipeService );
        SERVICE_DESTROY( AnalyticsService );
        SERVICE_DESTROY( StatisticService );

#if defined(MENGINE_PLATFORM_WINDOWS)
        SERVICE_DESTROY( Win32KernelService );
#endif

#if defined(MENGINE_PLATFORM_ANDROID)
        SERVICE_DESTROY( AndroidAssetService );
        SERVICE_DESTROY( AndroidKernelService );
#endif

#if defined(MENGINE_VENDOR_APPLE)
        SERVICE_DESTROY( AppleKernelService );
#endif

#if defined(MENGINE_PLATFORM_IOS)
        SERVICE_DESTROY( iOSKernelService );
#endif

        SERVICE_DESTROY( EnvironmentService );

        SERVICE_DESTROY( PlatformService );
        SERVICE_DESTROY( NotificationService );
        SERVICE_DESTROY( LoggerService );
        SERVICE_DESTROY( FactoryService );
        SERVICE_DESTROY( OptionsService );
        SERVICE_DESTROY( LifecycleService );
        SERVICE_DESTROY( CryptographySystem );
        SERVICE_DESTROY( PlatformSystem );
        SERVICE_DESTROY( ThreadSystem );
    }
    //////////////////////////////////////////////////////////////////////////
}
