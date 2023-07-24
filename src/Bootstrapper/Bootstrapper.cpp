#include "Bootstrapper.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/ModuleServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/FrameworkInterface.h"
#include "Interface/AccountServiceInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/GameServiceInterface.h"
#include "Interface/FrameworkServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/AnalyticsServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/VectorConstString.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionVocabulary.h"
#include "Kernel/Stringalized.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/VectorString.h"
#include "Kernel/Stringstream.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/FileLogger.h"
#include "Kernel/BuildMode.h"
#include "Kernel/FilePathDateTimeHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/TimestampHelper.h"

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
SERVICE_EXTERN( SecureService );
SERVICE_EXTERN( FactoryService );
SERVICE_EXTERN( PersistentSystem );
SERVICE_EXTERN( DateTimeSystem );
SERVICE_EXTERN( OptionsService );
SERVICE_EXTERN( LoggerService );
SERVICE_EXTERN( PlatformSystem );
SERVICE_EXTERN( PlatformService );
SERVICE_EXTERN( NotificationService );
SERVICE_EXTERN( UnicodeSystem );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( ConfigService );
SERVICE_EXTERN( SettingsService );
SERVICE_EXTERN( ArchiveService );
SERVICE_EXTERN( AnalyticsService );
SERVICE_EXTERN( ThreadService );
SERVICE_EXTERN( ThreadSystem );
SERVICE_EXTERN( MockupRenderSystem );
SERVICE_EXTERN( RenderSystem );
SERVICE_EXTERN( SoundService );
SERVICE_EXTERN( SoundSystem );
SERVICE_EXTERN( SilentSoundSystem );
SERVICE_EXTERN( ModuleService );
SERVICE_EXTERN( FrameworkService );
SERVICE_EXTERN( CodecService );
SERVICE_EXTERN( DataService );
SERVICE_EXTERN( PrefetcherService );
SERVICE_EXTERN( MemoryService );
SERVICE_EXTERN( ConverterService );
SERVICE_EXTERN( InputService );
SERVICE_EXTERN( TimeSystem );
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
SERVICE_EXTERN( GameService );
SERVICE_EXTERN( TimelineService );
SERVICE_EXTERN( TimepipeService );
SERVICE_EXTERN( StatisticService );
SERVICE_EXTERN( Application );
SERVICE_EXTERN( EnumeratorService );
SERVICE_EXTERN( ChronometerService );
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_ANDROID)
SERVICE_EXTERN( AndroidEnvironmentService );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_APPLE)
SERVICE_EXTERN( AppleEnvironmentService );
#endif
//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_EXTERNAL_SOURCE)
PLUGIN_EXPORT( ExternalBootstrapper );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_ZIP_STATIC
PLUGIN_EXPORT( Zip );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_LZ4_STATIC
PLUGIN_EXPORT( LZ4 );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_INI_STATIC
PLUGIN_EXPORT( INI );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_IMAGE_CODEC_STATIC
PLUGIN_EXPORT( ImageCodec );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_WAV_STATIC
PLUGIN_EXPORT( WAV );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_AMPLIFIER_STATIC
PLUGIN_EXPORT( Amplifier );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_OGG_VORBIS_STATIC
PLUGIN_EXPORT( OggVorbis );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_ASTRALAX_STATIC
PLUGIN_EXPORT( Astralax );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_DAZZLE_STATIC
PLUGIN_EXPORT( Dazzle );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_BITMAPFONT_STATIC
PLUGIN_EXPORT( BitmapFont );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_NODEDEBUGRENDER_STATIC
PLUGIN_EXPORT( NodeDebugRender );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_DEBUGPANEL_STATIC
PLUGIN_EXPORT( DebugPanel );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_RESOURCEPREFETCHER_STATIC
PLUGIN_EXPORT( ResourcePrefetcher );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_RESOURCECONVERT_STATIC
PLUGIN_EXPORT( ResourceConvert );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_RESOURCEVALIDATE_STATIC
PLUGIN_EXPORT( ResourceValidate );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_RESOURCEDEBUGGER_STATIC
PLUGIN_EXPORT( ResourceDebugger );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_METABUFLOADER_STATIC
PLUGIN_EXPORT( MetabufLoader );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_PVRTC_STATIC
PLUGIN_EXPORT( PVRTC );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_ETC1_STATIC
PLUGIN_EXPORT( ETC1 );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_DDS_STATIC
PLUGIN_EXPORT( DDS );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_JPEG_STATIC
PLUGIN_EXPORT( JPEG );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_PNG_STATIC
PLUGIN_EXPORT( PNG );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_WEBP_STATIC
PLUGIN_EXPORT( WebP );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_VIDEO_STATIC
PLUGIN_EXPORT( Video );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_THEORA_STATIC
PLUGIN_EXPORT( Theora );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_CURL_STATIC
PLUGIN_EXPORT( cURL );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_SPINE_STATIC
PLUGIN_EXPORT( Spine );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_MOVIE_STATIC
PLUGIN_EXPORT( Movie );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_MOVIE1_STATIC
PLUGIN_EXPORT( Movie1 );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_TTF_STATIC
PLUGIN_EXPORT( TTF );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_FE_STATIC
PLUGIN_EXPORT( FE );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_WIN32_SOCKET_STATIC
PLUGIN_EXPORT( Win32Socket );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_SDL_SOCKET_STATIC
PLUGIN_EXPORT( SDLSocket );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_NODEDEBUGGER_STATIC
PLUGIN_EXPORT( NodeDebugger );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_OZZ_ANIMATION_STATIC
PLUGIN_EXPORT( OzzAnimation );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_PYTHONFRAMEWORK_STATIC
PLUGIN_EXPORT( PythonFramework );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_UIFRAMEWORK_STATIC
PLUGIN_EXPORT( UIFramework );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_JSON_STATIC
PLUGIN_EXPORT( JSON );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_BOX2D_STATIC
PLUGIN_EXPORT( Box2D );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_TEXTUREPACKER_STATIC
PLUGIN_EXPORT( Texturepacker );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_GRAPHICS_STATIC
PLUGIN_EXPORT( Graphics );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_WIN32_SENTRY_STATIC
PLUGIN_EXPORT( Win32Sentry );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_STEAM_STATIC
PLUGIN_EXPORT( Steam );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_GOAP_STATIC
PLUGIN_EXPORT( GOAP );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_DEVTODEBUG_STATIC
PLUGIN_EXPORT( DevToDebug );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_IMGUI_STATIC
PLUGIN_EXPORT( ImGUI );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_CAMERADEBUGGIZMO_STATIC
PLUGIN_EXPORT( CameraDebugGizmo );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_FILEMODIFYHOOK_STATIC
PLUGIN_EXPORT( FileModifyHook );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_WIN32_ANTIFREEZEMONITOR_STATIC
PLUGIN_EXPORT( Win32AntifreezeMonitor );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_PYTHON_STATIC
PLUGIN_EXPORT( AndroidNativePython );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_OPTICK_STATIC
PLUGIN_EXPORT( Optick );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_GAMEANALYTICS_STATIC
PLUGIN_EXPORT( GameAnalytics );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_USERNOTIFICATIONCENTER_STATIC
PLUGIN_EXPORT( AppleUserNotificationCenter );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_SKADNETWORK_STATIC
PLUGIN_EXPORT( AppleSKAdNetwork );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_GAMECENTER_STATIC
PLUGIN_EXPORT( AppleGameCenter );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_APPTRACKING_STATIC
PLUGIN_EXPORT( AppleAppTracking );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_MARSDK_STATIC
PLUGIN_EXPORT( AppleMARSDK );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_STATIC
PLUGIN_EXPORT( AppleAppLovin );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_FIREBASE_STATIC
PLUGIN_EXPORT( AppleFirebase );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_FIREBASE_ANALYTICS_STATIC
PLUGIN_EXPORT( AppleFirebaseAnalytics );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_FIREBASE_CRASHLYTICS_STATIC
PLUGIN_EXPORT( AppleFirebaseCrashlytics );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_FIREBASE_MESSAGING_STATIC
PLUGIN_EXPORT( AppleFirebaseMessaging );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_FIREBASE_REMOTECONFIG_STATIC
PLUGIN_EXPORT( AppleFirebaseRemoteConfig );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_FACEBOOK_STATIC
PLUGIN_EXPORT( AppleFacebook );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_HELPSHIFT_STATIC
PLUGIN_EXPORT( AppleHelpshift );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_STOREREVIEW_STATIC
PLUGIN_EXPORT( AppleStoreReview );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_STOREINAPPPURCHASE_STATIC
PLUGIN_EXPORT( AppleStoreInAppPurchase );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_ADJUST_STATIC
PLUGIN_EXPORT( AppleAdjust );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_DEVTODEV_STATIC
PLUGIN_EXPORT( AppleDevToDev );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_SENTRY_STATIC
PLUGIN_EXPORT( AppleSentry );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_XMLTOBIN_STATIC
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
    bool Bootstrapper::run()
    {
        if( this->createServices_() == false )
        {
            LOGGER_ERROR( "invalid create create services" );

            return false;
        }

        Timestamp mengine_run_timestamp = ANALYTICS_SERVICE()
            ->buildEvent( STRINGIZE_STRING_LOCAL( "mng_run_start" ), MENGINE_DOCUMENT_FACTORABLE )
            ->log();

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

        LOGGER_INFO( "bootstrapper", "bootstrapper create frameworks" );

        if( this->createFrameworks_() == false )
        {
            LOGGER_ERROR( "invalid create frameworks" );

            return false;
        }

        LOGGER_INFO( "bootstrapper", "bootstrapper run frameworks" );

        if( this->runFrameworks_() == false )
        {
            LOGGER_ERROR( "invalid run frameworks" );

            return false;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_RUN_FRAMEWORKS );

        LOGGER_INFO( "bootstrapper", "bootstrapper initialize game" );

        const FileGroupInterfacePtr & defaultFileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        if( APPLICATION_SERVICE()
            ->initializeGame( defaultFileGroup, m_packagesPaths, m_settingsPaths ) == false )
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

        ANALYTICS_SERVICE()
            ->buildEvent( STRINGIZE_STRING_LOCAL( "mng_run_completed" ), MENGINE_DOCUMENT_FACTORABLE )
            ->addParameterInteger( STRINGIZE_STRING_LOCAL( "time" ), Helper::getDurationTimestamp( mengine_run_timestamp ) )
            ->log();

        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_RUN_COMPLETE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        ConfigInterfacePtr loadApplicationConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentInterfacePtr & _doc )
        {
            if( _fileGroup->existFile( _filePath, true ) == false )
            {
                return nullptr;
            }

            ConfigInterfacePtr applicationConfig = CONFIG_SERVICE()
                ->loadConfig( _fileGroup, _filePath, ConstString::none(), _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( applicationConfig, "invalid open application settings '%s'"
                , _filePath.c_str()
            );

            return applicationConfig;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::loadApplicationConfig_()
    {
        FilePath applicationConfigPath = Helper::stringizeFilePath( MENGINE_BOOTSTRAPPER_CONFIG_PATH );

        const FileGroupInterfacePtr & defaultFileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        ConfigInterfacePtr applicationConfig = Detail::loadApplicationConfig( defaultFileGroup, applicationConfigPath, MENGINE_DOCUMENT_FACTORABLE );

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
            if( CONFIG_SERVICE()
                ->loadDefaultConfig( defaultFileGroup, filePath, ConstString::none(), MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalid load include config '%s'"
                    , filePath.c_str()
                );

                return false;
            }
        }

        VectorFilePath credentialsPaths;
        applicationConfig->getValues( "Credentials", "Path", &credentialsPaths );

        for( const FilePath & filePath : credentialsPaths )
        {
            if( CONFIG_SERVICE()
                ->loadDefaultConfig( defaultFileGroup, filePath, ConstString::none(), MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalid load credential '%s'"
                    , filePath.c_str()
                );

                return false;
            }
        }

        applicationConfig->getValues( "Packages", "Path", &m_packagesPaths );
        applicationConfig->getValues( "Settings", "Path", &m_settingsPaths );

        const Char * option_config = GET_OPTION_VALUE( "config", nullptr );

        if( option_config != nullptr )
        {
            if( CONFIG_SERVICE()
                ->loadDefaultConfig( defaultFileGroup, Helper::stringizeFilePath( option_config ), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalid load option config '%s'"
                    , option_config
                );

                return false;
            }
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
        Char userPath[MENGINE_MAX_PATH] = {'\0'};
        size_t userPathLen = PLATFORM_SERVICE()
            ->getUserPath( userPath );

        FilePath cs_userPath = Helper::stringizeFilePathSize( userPath, (FilePath::size_type)userPathLen );

        bool OPTION_showuserfolder = HAS_OPTION( "showuserfolder" );

        if( OPTION_showuserfolder == true )
        {
            PLATFORM_SERVICE()
                ->messageBox( "Mengine", "user folder: %s", cs_userPath.c_str() );
        }
        else
        {
            LOGGER_INFO_PROTECTED( "bootstrapper", "user folder: %s", cs_userPath.c_str() );
        }

        // mount user directory
        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "user" ), nullptr, nullptr, cs_userPath, STRINGIZE_STRING_LOCAL( "global" ), nullptr, true, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "failed to mount user directory '%s'"
                , userPath
            );

            return false;
        }

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

        Char filePathDate[MENGINE_MAX_PATH] = {'\0'};
        Helper::makeFilePathDateTimestamp( filePathDate, MENGINE_MAX_PATH );

        WString unicode_date;
        Helper::utf8ToUnicode( filePathDate, &unicode_date );

        WString unicode_logFilename;
        unicode_logFilename += L"Game";

        bool developmentMode = Helper::isDevelopmentMode();
        bool OPTION_roaming = HAS_OPTION( "roaming" );
        bool OPTION_noroaming = HAS_OPTION( "noroaming" );

        if( developmentMode == true && (OPTION_roaming == false || OPTION_noroaming == false) )
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

        FileLoggerPtr fileLog = Helper::makeFactorableUnique<FileLogger>( MENGINE_DOCUMENT_FACTORABLE );

        fileLog->setFileGroup( userFileGroup );
        fileLog->setFilePath( logFilename );

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

            Char fullLogFilename[MENGINE_MAX_PATH] = {'\0'};
            userFileGroup->getFullPath( logFilename, fullLogFilename );

            LOGGER_INFO_PROTECTED( "bootstrapper", "write file log to: %s"
                , fullLogFilename
            );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_ADD_PLUGIN( Name, Info, Doc )\
        {\
            if(SERVICE_IS_INITIALIZE(LoggerServiceInterface) == true)\
            {\
                LOGGER_INFO( "bootstrapper", "%s", Info );\
            }\
            if( PLUGIN_CREATE(Name, Doc) == false )\
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
        MENGINE_ADD_SERVICE( EnumeratorService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( PlatformSystem, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( ThreadSystem, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( UnicodeSystem, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( TimeSystem, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( DateTimeSystem, MENGINE_DOCUMENT_FACTORABLE );        
        MENGINE_ADD_SERVICE( PersistentSystem, MENGINE_DOCUMENT_FACTORABLE );        
        MENGINE_ADD_SERVICE( OptionsService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( FactoryService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( PrototypeService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( MemoryService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( NotificationService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( VocabularyService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( LoggerService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( SecureService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( AnalyticsService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( StatisticService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( TimelineService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( TimepipeService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( PluginService, MENGINE_DOCUMENT_FACTORABLE );

#if defined(MENGINE_PLATFORM_ANDROID)
        MENGINE_ADD_SERVICE( AndroidEnvironmentService, MENGINE_DOCUMENT_FACTORABLE );
#endif

#if defined(MENGINE_PLATFORM_APPLE)
        MENGINE_ADD_SERVICE( AppleEnvironmentService, MENGINE_DOCUMENT_FACTORABLE );
#endif


        MENGINE_ADD_SERVICE( PlatformService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( FileService, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_SERVICE( ConfigService, MENGINE_DOCUMENT_FACTORABLE );

        LOGGER_INFO( "bootstrapper", "debug mode [%s]", Helper::isDebugMode() == true ? "ON" : "OFF" );
        LOGGER_INFO( "bootstrapper", "development mode [%s]", Helper::isDevelopmentMode() == true ? "ON" : "OFF" );
        LOGGER_INFO( "bootstrapper", "build publish [%s]", Helper::isBuildPublish() == true ? "ON" : "OFF" );
        LOGGER_INFO( "bootstrapper", "master release [%s]", Helper::isMasterRelease() == true ? "ON" : "OFF" );

#ifdef MENGINE_ASSERTION_DEBUG_ENABLE
        LOGGER_INFO( "bootstrapper", "enable assertion debug [ON]" );
#else
        LOGGER_INFO( "bootstrapper", "enable assertion debug [OFF]" );
#endif

#ifdef MENGINE_LOGGER_ANALYZE_ENABLE
        LOGGER_INFO( "bootstrapper", "enable logger analyze [ON]" );
#else
        LOGGER_INFO( "bootstrapper", "enable logger analyze [OFF]" );
#endif

#ifdef MENGINE_ALLOCATOR_DEBUG_ENABLE
        LOGGER_INFO( "bootstrapper", "enable allocator debug [ON]" );
#else
        LOGGER_INFO( "bootstrapper", "enable allocator debug [OFF]" );
#endif

#if defined(MENGINE_DOCUMENT_ENABLE)
        LOGGER_INFO( "bootstrapper", "enable document debug [ON]" );
#else
        LOGGER_INFO( "bootstrapper", "enable document debug [OFF]" );
#endif

#ifdef MENGINE_PLUGIN_INI_STATIC
        MENGINE_ADD_PLUGIN( INI, "plugin INI...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_JSON_STATIC
        MENGINE_ADD_PLUGIN( JSON, "plugin JSON...", MENGINE_DOCUMENT_FACTORABLE );
#endif

        Char currentPath[MENGINE_MAX_PATH] = {'\0'};
        PLATFORM_SERVICE()
            ->getCurrentPath( currentPath );

        LOGGER_INFO_PROTECTED( "bootstrapper", "current folder: %s"
            , currentPath
        );

#ifdef MENGINE_BOOTSTRAPPER_LOAD_CONFIG_ENABLE
        LOGGER_INFO( "bootstrapper", "bootstrapper load application config" );

        if( this->loadApplicationConfig_() == false )
        {
            LOGGER_ERROR( "invalid load application config" );

            return false;
        }
#endif

        bool OPTION_assertion = HAS_OPTION( "assertion" );
        bool Engine_AssertionDebugBreak = CONFIG_VALUE( "Engine", "AssertionDebugBreak", false );

        bool Assertion_NoDebugBreak = OPTION_assertion == false && Engine_AssertionDebugBreak == false;

        Helper::AssertionSetNotDebugBreak( Assertion_NoDebugBreak );

        LOGGER_INFO( "bootstrapper", "bootstrapper mount user file group" );

        if( this->mountUserFileGroup_() == false )
        {
            LOGGER_ERROR( "invalid mount user file group" );

            return false;
        }

#ifdef MENGINE_PLUGIN_OPTICK_STATIC
        MENGINE_ADD_PLUGIN( Optick, "initialize Optick...", MENGINE_DOCUMENT_FACTORABLE );
#endif

        Timestamp mengine_initialize_services_timestamp = ANALYTICS_SERVICE()
            ->buildEvent( STRINGIZE_STRING_LOCAL( "mng_initialize_services_start" ), MENGINE_DOCUMENT_FACTORABLE )
            ->log();

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

#ifdef MENGINE_PLUGIN_ZIP_STATIC
        MENGINE_ADD_PLUGIN( Zip, "plugin Zip...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_LZ4_STATIC
        MENGINE_ADD_PLUGIN( LZ4, "plugin LZ4...", MENGINE_DOCUMENT_FACTORABLE );
#endif

        bool OPTION_norender = HAS_OPTION( "norender" );

        if( OPTION_norender == true )
        {
            BOOTSTRAPPER_SERVICE_CREATE( MockupRenderSystem, MENGINE_DOCUMENT_FACTORABLE );
        }
        else
        {
            BOOTSTRAPPER_SERVICE_CREATE( RenderSystem, MENGINE_DOCUMENT_FACTORABLE );
        }

        BOOTSTRAPPER_SERVICE_CREATE( RenderService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( RenderMaterialService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( RenderTextureService, MENGINE_DOCUMENT_FACTORABLE );

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

        BOOTSTRAPPER_SERVICE_CREATE( SoundService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( ModuleService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( FrameworkService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( CodecService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( DataService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( PrefetcherService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( ConverterService, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( InputService, MENGINE_DOCUMENT_FACTORABLE );
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

#undef BOOTSTRAPPER_SERVICE_CREATE

        ANALYTICS_SERVICE()
            ->buildEvent( STRINGIZE_STRING_LOCAL( "mng_initialize_services_completed" ), MENGINE_DOCUMENT_FACTORABLE )
            ->addParameterInteger( STRINGIZE_STRING_LOCAL( "time" ), Helper::getDurationTimestamp( mengine_initialize_services_timestamp ) )
            ->log();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createStaticPlugins_()
    {
        LOGGER_INFO( "bootstrapper", "create plugins..." );

#ifdef MENGINE_PLUGIN_WIN32_SENTRY_STATIC
        MENGINE_ADD_PLUGIN( Win32Sentry, "plugin Win32Sentry...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_SENTRY_STATIC
        MENGINE_ADD_PLUGIN( AppleSentry, "plugin AppleSentry...", MENGINE_DOCUMENT_FACTORABLE );
#endif        

#ifdef MENGINE_EXTERNAL_SOURCE
        MENGINE_ADD_PLUGIN( ExternalBootstrapper, "initialize external Bootstrapper...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_PYTHONFRAMEWORK_STATIC
        MENGINE_ADD_PLUGIN( PythonFramework, "plugin PythonFramework...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_UIFRAMEWORK_STATIC
        MENGINE_ADD_PLUGIN( UIFramework, "plugin UIFramework...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_NODEDEBUGRENDER_STATIC
        MENGINE_ADD_PLUGIN( NodeDebugRender, "plugin NodeDebugRender...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_DEBUGPANEL_STATIC
        MENGINE_ADD_PLUGIN( DebugPanel, "plugin DebugPanel...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEPREFETCHER_STATIC
        MENGINE_ADD_PLUGIN( ResourcePrefetcher, "plugin ResourcePrefetcher...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_RESOURCECONVERT_STATIC
        MENGINE_ADD_PLUGIN( ResourceConvert, "plugin ResourceConvert...", MENGINE_DOCUMENT_FACTORABLE );
#endif        

#ifdef MENGINE_PLUGIN_RESOURCEVALIDATE_STATIC
        MENGINE_ADD_PLUGIN( ResourceValidate, "plugin ResourceValidate...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEDEBUGGER_STATIC
        MENGINE_ADD_PLUGIN( ResourceDebugger, "plugin ResourceDebugger...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_NODEDEBUGGER_STATIC
        MENGINE_ADD_PLUGIN( NodeDebugger, "plugin NodeDebugger...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_OZZ_ANIMATION_STATIC
        MENGINE_ADD_PLUGIN( OzzAnimation, "plugin OzzAnimation...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_BOX2D_STATIC
        MENGINE_ADD_PLUGIN( Box2D, "plugin Box2D...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_TEXTUREPACKER_STATIC
        MENGINE_ADD_PLUGIN( Texturepacker, "plugin Texturepacker...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_GRAPHICS_STATIC
        MENGINE_ADD_PLUGIN( Graphics, "plugin Graphics...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_METABUFLOADER_STATIC
        MENGINE_ADD_PLUGIN( MetabufLoader, "plugin MetabufLoader...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_IMAGE_CODEC_STATIC
        MENGINE_ADD_PLUGIN( ImageCodec, "plugin ImageCodec...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_WAV_STATIC
        MENGINE_ADD_PLUGIN( WAV, "plugin WAVCodec...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_OGG_VORBIS_STATIC
        MENGINE_ADD_PLUGIN( OggVorbis, "plugin OggVorbisCodec...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_AMPLIFIER_STATIC
        MENGINE_ADD_PLUGIN( Amplifier, "plugin Amplifier...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_PVRTC_STATIC
        MENGINE_ADD_PLUGIN( PVRTC, "plugin PVRTC...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_ETC1_STATIC
        MENGINE_ADD_PLUGIN( ETC1, "plugin ETC1...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_DDS_STATIC
        MENGINE_ADD_PLUGIN( DDS, "plugin DDS...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_JPEG_STATIC
        MENGINE_ADD_PLUGIN( JPEG, "plugin JPEG...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_PNG_STATIC
        MENGINE_ADD_PLUGIN( PNG, "plugin PNG...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_WEBP_STATIC
        MENGINE_ADD_PLUGIN( WebP, "plugin WebP...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_VIDEO_STATIC
        MENGINE_ADD_PLUGIN( Video, "plugin Video...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_THEORA_STATIC
        MENGINE_ADD_PLUGIN( Theora, "plugin Theora...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_CURL_STATIC
        MENGINE_ADD_PLUGIN( cURL, "plugin cURL...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_SPINE_STATIC
        MENGINE_ADD_PLUGIN( Spine, "plugin Spine...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_MOVIE_STATIC
        MENGINE_ADD_PLUGIN( Movie, "plugin Movie...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_MOVIE1_STATIC
        MENGINE_ADD_PLUGIN( Movie1, "plugin Movie1...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_ASTRALAX_STATIC
        MENGINE_ADD_PLUGIN( Astralax, "plugin Astralax...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_DAZZLE_STATIC
        MENGINE_ADD_PLUGIN( Dazzle, "plugin Dazzle...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_BITMAPFONT_STATIC
        MENGINE_ADD_PLUGIN( BitmapFont, "plugin BitmapFont...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_TTF_STATIC
        MENGINE_ADD_PLUGIN( TTF, "plugin TTF...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_FE_STATIC
        MENGINE_ADD_PLUGIN( FE, "plugin FE...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_STEAM_STATIC
        MENGINE_ADD_PLUGIN( Steam, "plugin Steam...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_WIN32_SOCKET_STATIC
        MENGINE_ADD_PLUGIN( Win32Socket, "plugin Win32Socket...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_SDL_SOCKET_STATIC
        MENGINE_ADD_PLUGIN( SDLSocket, "plugin SDLSocket...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_GOAP_STATIC
        MENGINE_ADD_PLUGIN( GOAP, "plugin GOAP...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_DEVTODEBUG_STATIC
        MENGINE_ADD_PLUGIN( DevToDebug, "plugin DevToDebug...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_IMGUI_STATIC
        MENGINE_ADD_PLUGIN( ImGUI, "plugin ImGUI...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_CAMERADEBUGGIZMO_STATIC
        MENGINE_ADD_PLUGIN( CameraDebugGizmo, "plugin CameraDebugGizmo...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_FILEMODIFYHOOK_STATIC
        MENGINE_ADD_PLUGIN( FileModifyHook, "plugin FileModifyHook...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_WIN32_CONSOLELOGGER_STATIC
        MENGINE_ADD_PLUGIN( Win32ConsoleLogger, "plugin Win32ConsoleLogger...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_WIN32_ANTIFREEZEMONITOR_STATIC
        MENGINE_ADD_PLUGIN( Win32AntifreezeMonitor, "plugin Win32AntifreezeMonitor...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_PYTHON_STATIC
        MENGINE_ADD_PLUGIN( AndroidNativePython, "plugin AndroidKernelPython...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_GAMEANALYTICS_STATIC
        MENGINE_ADD_PLUGIN( GameAnalytics, "plugin GameAnalytics...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_USERNOTIFICATIONCENTER_STATIC
        MENGINE_ADD_PLUGIN( AppleUserNotificationCenter, "plugin AppleUserNotificationCenter...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_SKADNETWORK_STATIC
        MENGINE_ADD_PLUGIN( AppleSKAdNetwork, "plugin AppleSKAdNetwork...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_GAMECENTER_STATIC
        MENGINE_ADD_PLUGIN( AppleGameCenter, "plugin AppleGameCenter...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_APPTRACKING_STATIC
        MENGINE_ADD_PLUGIN( AppleAppTracking, "plugin AppleAppTracking...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_MARSDK_STATIC
        MENGINE_ADD_PLUGIN( AppleMARSDK, "plugin AppleMARSDK...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_APPLOVIN_STATIC
        MENGINE_ADD_PLUGIN( AppleAppLovin, "plugin AppleAppLovin...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_FIREBASE_STATIC
        MENGINE_ADD_PLUGIN( AppleFirebase, "plugin AppleFirebase...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_FIREBASE_ANALYTICS_STATIC
        MENGINE_ADD_PLUGIN( AppleFirebaseAnalytics, "plugin AppleFirebaseAnalytics...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_FIREBASE_CRASHLYTICS_STATIC
        MENGINE_ADD_PLUGIN( AppleFirebaseCrashlytics, "plugin AppleFirebaseCrashlytics...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_FIREBASE_MESSAGING_STATIC
        MENGINE_ADD_PLUGIN( AppleFirebaseMessaging, "plugin AppleFirebaseMessaging...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_FIREBASE_REMOTECONFIG_STATIC
        MENGINE_ADD_PLUGIN( AppleFirebaseRemoteConfig, "plugin AppleFirebaseRemoteConfig...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_FACEBOOK_STATIC
        MENGINE_ADD_PLUGIN( AppleFacebook, "plugin AppleFacebook...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_HELPSHIFT_STATIC
        MENGINE_ADD_PLUGIN( AppleHelpshift, "plugin AppleHelpshift...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_STOREREVIEW_STATIC
        MENGINE_ADD_PLUGIN( AppleStoreReview, "plugin AppleStoreReview...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_STOREINAPPPURCHASE_STATIC
        MENGINE_ADD_PLUGIN( AppleStoreInAppPurchase, "plugin AppleStoreInAppPurchase...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_ADJUST_STATIC
        MENGINE_ADD_PLUGIN( AppleAdjust, "plugin AppleAdjust...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_DEVTODEV_STATIC
        MENGINE_ADD_PLUGIN( AppleDevToDev, "plugin AppleDevToDev...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_XMLTOBIN_STATIC
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
            LOGGER_INFO( "bootstrapper", "create dynamic plugin: %s"
                , pluginName.c_str()
            );

            if( PLUGIN_SERVICE()
                ->loadPlugin( pluginName.c_str(), MENGINE_DOCUMENT_FACTORABLE ) == false )
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
        bool devplugins = MENGINE_MASTER_RELEASE_VALUE( false, Helper::isDevelopmentMode() );

        if( HAS_OPTION( "devplugins" ) == true )
        {
            devplugins = true;
        }

        if( HAS_OPTION( "nodevplugins" ) == true )
        {
            devplugins = false;
        }

        if( devplugins == true )
        {
            VectorString devPlugins;
            CONFIG_VALUES( "DevPlugins", "Name", &devPlugins );

            for( const String & pluginName : devPlugins )
            {
                LOGGER_INFO( "bootstrapper", "create dynamic dev plugin: %s"
                    , pluginName.c_str()
                );

                if( PLUGIN_SERVICE()
                    ->loadPlugin( pluginName.c_str(), MENGINE_DOCUMENT_FACTORABLE ) == false )
                {
                    LOGGER_WARNING( "failed to load dynamic dev plugin '%s'"
                        , pluginName.c_str()
                    );
                }
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
            LOGGER_INFO( "bootstrapper", "create dynamic system plugin: %s"
                , pluginName.c_str()
            );

            if( PLUGIN_SERVICE()
                ->loadPlugin( pluginName.c_str(), MENGINE_DOCUMENT_FACTORABLE ) == false )
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

        if( devplugins == true )
        {
            VectorString devPlugins;
            CONFIG_VALUES( "SystemDevPlugins", "Name", &devPlugins );

            for( const String & pluginName : devPlugins )
            {
                LOGGER_INFO( "bootstrapper", "create dynamic system dev plugin: %s"
                    , pluginName.c_str()
                );

                if( PLUGIN_SERVICE()
                    ->loadPlugin( pluginName.c_str(), MENGINE_DOCUMENT_FACTORABLE ) == false )
                {
                    LOGGER_WARNING( "failed to load dynamic system dev plugin '%s'"
                        , pluginName.c_str()
                    );
                }
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
            LOGGER_INFO( "bootstrapper", "create dynamic priority plugin: %s"
                , pluginName.c_str()
            );

            if( PLUGIN_SERVICE()
                ->loadPlugin( pluginName.c_str(), MENGINE_DOCUMENT_FACTORABLE ) == false )
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

        if( devplugins == true )
        {
            VectorString devPlugins;
            CONFIG_VALUES( "PriorityDevPlugins", "Name", &devPlugins );

            for( const String & pluginName : devPlugins )
            {
                LOGGER_INFO( "bootstrapper", "create dynamic priority dev plugin: %s"
                    , pluginName.c_str()
                );

                if( PLUGIN_SERVICE()
                    ->loadPlugin( pluginName.c_str(), MENGINE_DOCUMENT_FACTORABLE ) == false )
                {
                    LOGGER_WARNING( "failed to load priority dev plugin '%s'"
                        , pluginName.c_str()
                    );
                }
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createApplication_()
    {
        LOGGER_INFO( "bootstrapper", "create application..." );

        Timestamp mengine_create_application_timestamp = ANALYTICS_SERVICE()
            ->buildEvent( STRINGIZE_STRING_LOCAL( "mng_create_application_start" ), MENGINE_DOCUMENT_FACTORABLE )
            ->log();

        if( SERVICE_CREATE_SAFE( Application, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );

        ANALYTICS_SERVICE()
            ->buildEvent( STRINGIZE_STRING_LOCAL( "mng_create_application_completed" ), MENGINE_DOCUMENT_FACTORABLE )
            ->addParameterInteger( STRINGIZE_STRING_LOCAL( "time" ), Helper::getDurationTimestamp( mengine_create_application_timestamp ) )
            ->log();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createFrameworks_()
    {
        LOGGER_INFO( "bootstrapper", "create frameworks..." );

        VectorConstString frameworks;
        CONFIG_VALUES( "Frameworks", "Name", &frameworks );

        for( const ConstString & frameworkName : frameworks )
        {
            LOGGER_INFO( "bootstrapper", "create framework: %s"
                , frameworkName.c_str()
            );

            if( FRAMEWORK_SERVICE()
                ->initializeFramework( frameworkName, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "failed initialize framework '%s'"
                    , frameworkName.c_str()
                );

                return false;
            }
        }

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
            LOGGER_INFO( "bootstrapper", "run module: %s"
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
#if defined(MENGINE_MASTER_RELEASE)
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

                LOGGER_INFO( "bootstrapper", "run dev module: %s"
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
    bool Bootstrapper::runFrameworks_()
    {
        if( FRAMEWORK_SERVICE()
            ->runFrameworks() == false )
        {
            return false;
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

            LOGGER_INFO( "bootstrapper", "stop module: %s"
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

#if defined(MENGINE_MASTER_RELEASE)
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

                LOGGER_INFO( "bootstrapper", "stop dev module: %s"
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
    void Bootstrapper::finalizeFrameworks_()
    {
        if( SERVICE_IS_INITIALIZE( FrameworkServiceInterface ) == false )
        {
            return;
        }

        VectorConstString frameworks;
        CONFIG_VALUES( "Frameworks", "Name", &frameworks );

        for( const ConstString & frameworkName : frameworks )
        {
            if( FRAMEWORK_SERVICE()
                ->isInitializeFramework( frameworkName ) == false )
            {
                continue;
            }

            LOGGER_INFO( "bootstrapper", "finalize framework: %s"
                , frameworkName.c_str()
            );

            if( FRAMEWORK_SERVICE()
                ->finalizeFramework( frameworkName ) == false )
            {
                LOGGER_ERROR( "failed to stop framework '%s'"
                    , frameworkName.c_str()
                );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Bootstrapper::stopFrameworks_()
    {
        if( SERVICE_IS_INITIALIZE( FrameworkServiceInterface ) == false )
        {
            return;
        }

        FRAMEWORK_SERVICE()
            ->stopFrameworks();
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
        m_settingsPaths.clear();

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

        this->stopFrameworks_();
        this->finalizeFrameworks_();
        
        if( SERVICE_IS_INITIALIZE( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_ENGINE_STOP_THREADS );
        }

        if( SERVICE_IS_INITIALIZE( ThreadServiceInterface ) == true )
        {
            THREAD_SERVICE()
                ->stopTasks();
        }

        SERVICE_FINALIZE( GameService );
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
        SERVICE_FINALIZE( TimelineService );
        SERVICE_FINALIZE( WatchdogService );
        SERVICE_FINALIZE( ModuleService );
        SERVICE_FINALIZE( FrameworkService );
        SERVICE_FINALIZE( PlayerService );
        SERVICE_FINALIZE( PickerService );
        SERVICE_FINALIZE( UpdateService );
        SERVICE_FINALIZE( InputService );
        SERVICE_FINALIZE( CodecService );
        SERVICE_FINALIZE( SoundService );
        SERVICE_FINALIZE( ConverterService );
        SERVICE_FINALIZE( RenderService );
        SERVICE_FINALIZE( PrototypeService );
        SERVICE_FINALIZE( RenderMaterialService );
        SERVICE_FINALIZE( RenderTextureService );
        SERVICE_FINALIZE( ArchiveService );
        SERVICE_FINALIZE( MemoryService );
        SERVICE_FINALIZE( ThreadService );
        SERVICE_FINALIZE( EasingService );

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ) );

        SERVICE_FINALIZE( VocabularyService );
        SERVICE_FINALIZE( EnumeratorService );
        SERVICE_FINALIZE( PluginService );
        SERVICE_FINALIZE( TimepipeService );

        if( m_loggerFile != nullptr )
        {
            LOGGER_MESSAGE( "finish log successful" );

            LOGGER_SERVICE()
                ->unregisterLogger( m_loggerFile );

            m_loggerFile = nullptr;
        }

        if( SERVICE_IS_INITIALIZE( FileServiceInterface ) == true )
        {
            FILE_SERVICE()
                ->unmountFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );
        }

        SERVICE_FINALIZE( FileService );
        SERVICE_FINALIZE( RenderSystem );
        SERVICE_FINALIZE( SoundSystem );

#if defined(MENGINE_PLATFORM_ANDROID)
        SERVICE_FINALIZE( AndroidEnvironmentService );
#endif

#if defined(MENGINE_PLATFORM_APPLE)
        SERVICE_FINALIZE( AppleEnvironmentService );
#endif

        SERVICE_FINALIZE( AnalyticsService );
        SERVICE_FINALIZE( StatisticService );

        SERVICE_FINALIZE( PlatformService );
        SERVICE_FINALIZE( LoggerService );
        SERVICE_FINALIZE( FactoryService );
        SERVICE_FINALIZE( OptionsService );
        SERVICE_FINALIZE( NotificationService );
        SERVICE_FINALIZE( TimeSystem );
        SERVICE_FINALIZE( DateTimeSystem );
        SERVICE_FINALIZE( UnicodeSystem );
        SERVICE_FINALIZE( ThreadSystem );
        SERVICE_FINALIZE( PersistentSystem );
        SERVICE_FINALIZE( PlatformSystem );

        SERVICE_DESTROY( SceneService );
        SERVICE_DESTROY( GameService );
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
        SERVICE_DESTROY( TimelineService );
        SERVICE_DESTROY( WatchdogService );
        SERVICE_DESTROY( ModuleService );
        SERVICE_DESTROY( FrameworkService );
        SERVICE_DESTROY( PlayerService );
        SERVICE_DESTROY( PickerService );
        SERVICE_DESTROY( UpdateService );
        SERVICE_DESTROY( InputService );
        SERVICE_DESTROY( UnicodeSystem );
        SERVICE_DESTROY( CodecService );
        SERVICE_DESTROY( SoundService );
        SERVICE_DESTROY( SoundSystem );
        SERVICE_DESTROY( ConverterService );
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
        SERVICE_DESTROY( TimeSystem );
        SERVICE_DESTROY( DateTimeSystem );
        SERVICE_DESTROY( EasingService );
        SERVICE_DESTROY( VocabularyService );
        SERVICE_DESTROY( EnumeratorService );
        SERVICE_DESTROY( PluginService );
        SERVICE_DESTROY( FileService );
        SERVICE_DESTROY( ThreadSystem );
        SERVICE_DESTROY( TimepipeService );
        SERVICE_DESTROY( AnalyticsService );
        SERVICE_DESTROY( StatisticService );

#if defined(MENGINE_PLATFORM_ANDROID)
        SERVICE_DESTROY( AndroidEnvironmentService );
#endif

#if defined(MENGINE_PLATFORM_APPLE)
        SERVICE_DESTROY( AppleEnvironmentService );
#endif

        SERVICE_DESTROY( PlatformService );
        SERVICE_DESTROY( NotificationService );
        SERVICE_DESTROY( LoggerService );
        SERVICE_DESTROY( FactoryService );
        SERVICE_DESTROY( OptionsService );
        SERVICE_DESTROY( PersistentSystem );
        SERVICE_DESTROY( PlatformSystem );
    }
    //////////////////////////////////////////////////////////////////////////
}
