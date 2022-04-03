#include "Bootstrapper.h"

#include "Interface/PluginServiceInterface.h"
#include "Interface/ModuleServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/FrameworkInterface.h"
#include "Interface/AccountServiceInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/GameServiceInterface.h"
#include "Interface/FrameworkServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/LoggerServiceInterface.h"

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

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_APPLICATION_INI_PATH
#define MENGINE_APPLICATION_INI_PATH "application.ini"
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_APPLICATION_JSON_PATH
#define MENGINE_APPLICATION_JSON_PATH "application.json"
#endif
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( SecureService );
SERVICE_EXTERN( FactoryService );
SERVICE_EXTERN( OptionsService );
SERVICE_EXTERN( LoggerService );
SERVICE_EXTERN( Platform );
SERVICE_EXTERN( NotificationService );
SERVICE_EXTERN( UnicodeSystem );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( ConfigService );
SERVICE_EXTERN( SettingsService );
SERVICE_EXTERN( ArchiveService );
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
SERVICE_EXTERN( Application );
SERVICE_EXTERN( EnumeratorService );
SERVICE_EXTERN( ChronometerService );
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_EXTERNAL_SOURCE
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
#ifdef MENGINE_PLUGIN_SENTRY_STATIC
PLUGIN_EXPORT( Sentry );
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
#ifdef MENGINE_PLUGIN_APPLE_GAMECENTER_STATIC
PLUGIN_EXPORT( AppleGameCenter );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_APPTRACKING_STATIC
PLUGIN_EXPORT( AppleAppTracking );
#endif
//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_APPLE_MARSDK_STATIC
PLUGIN_EXPORT( MARSDK );
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
            LOGGER_CRITICAL( "Application invalid initialize game" );

            return false;
        }

        LOGGER_INFO( "bootstrapper", "bootstrapper load personality" );

        if( GAME_SERVICE()
            ->loadPersonality() == false )
        {
            LOGGER_CRITICAL( "Game invalid load personality" );

            return false;
        }

        LOGGER_INFO( "bootstrapper", "bootstrapper run" );

        LOGGER_SERVICE()
            ->setHistorically( false );

        LOGGER_SERVICE()
            ->clearHistory();

        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_RUN_COMPLETE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        ConfigInterfacePtr loadApplicationConfig( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc )
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
        FilePath applicationJsonPath = Helper::stringizeFilePath( MENGINE_APPLICATION_JSON_PATH );
        FilePath applicationIniPath = Helper::stringizeFilePath( MENGINE_APPLICATION_INI_PATH );

        const FileGroupInterfacePtr & defaultFileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        ConfigInterfacePtr applicationConfig = Detail::loadApplicationConfig( defaultFileGroup, applicationJsonPath, MENGINE_DOCUMENT_FACTORABLE );

        if( applicationConfig == nullptr )
        {
            applicationConfig = Detail::loadApplicationConfig( defaultFileGroup, applicationIniPath, MENGINE_DOCUMENT_FACTORABLE );
        }

        if( applicationConfig != nullptr )
        {

            VectorFilePath configsPaths;
            applicationConfig->getValues( "Configs", "Path", &configsPaths );

            for( const FilePath & filePath : configsPaths )
            {
                if( CONFIG_SERVICE()
                    ->loadDefaultConfig( defaultFileGroup, filePath, ConstString::none(), MENGINE_DOCUMENT_FACTORABLE ) == false )
                {
                    LOGGER_ERROR( "invalid load config '%s'"
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
        }
        else
        {
            LOGGER_INFO( "bootstrapper", "not exist application config '%s'"
                , applicationIniPath.c_str()
            );
        }

        const Char * option_config = GET_OPTION_VALUE( "config", nullptr );

        if( option_config != nullptr )
        {
            if( CONFIG_SERVICE()
                ->loadDefaultConfig( defaultFileGroup, Helper::stringizeFilePath( option_config ), ConstString::none(), MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalid load config '%s'"
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

        if( HAS_OPTION( "showuserfolder" ) == true )
        {
            PLATFORM_SERVICE()
                ->messageBox( "Mengine", "user folder: %s", cs_userPath.c_str() );
        }
        else
        {
            LOGGER_MESSAGE_RELEASE_PROTECTED( "User Folder: %s", cs_userPath.c_str() );
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

        DateTimeProviderInterfacePtr dateTimeProvider = PLATFORM_SERVICE()
            ->createDateTimeProvider( MENGINE_DOCUMENT_FACTORABLE );

        Char filePathDate[1024] = {'\0'};
        Helper::makeFilePathDateTimeHelper( dateTimeProvider, filePathDate, MENGINE_MAX_PATH );

        WString unicode_date;
        Helper::utf8ToUnicode( filePathDate, &unicode_date );

        WString unicode_logFilename;
        unicode_logFilename += L"Game";

        bool developmentMode = Helper::isDevelopmentMode();
        bool OPTION_roamingMode = HAS_OPTION( "roaming" );
        bool OPTION_noroamingMode = HAS_OPTION( "noroaming" );

        if( developmentMode == true && (OPTION_roamingMode == false || OPTION_noroamingMode == false) )
        {
            unicode_logFilename += L"_";
            unicode_logFilename += unicode_date;
        }

        unicode_logFilename += L".log";

        String utf8_logFilename;
        if( Helper::unicodeToUtf8( unicode_logFilename, &utf8_logFilename ) == false )
        {
            LOGGER_ERROR( "failed log directory '%ls' convert to ut8f"
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
            LOGGER_ERROR( "invalid register file logger '%s'"
                , logFilename.c_str()
            );
        }
        else
        {
            m_loggerFile = fileLog;

            Char fullLogFilename[MENGINE_MAX_PATH] = {'\0'};
            userFileGroup->getFullPath( logFilename, fullLogFilename );

            LOGGER_MESSAGE( "write file log to: %s"
                , fullLogFilename
            );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
#define MENGINE_ADD_PLUGIN( Name, Info, Doc )\
        {\
            LOGGER_MESSAGE( "%s", Info );\
            if( PLUGIN_CREATE(Name, Doc) == false )\
            {\
                LOGGER_ERROR( "%s [invalid initialize]", Info );\
            return false;\
            }\
        }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createServices_()
    {
        SERVICE_CREATE( EnumeratorService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( UnicodeSystem, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( OptionsService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( FactoryService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( PrototypeService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( MemoryService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( NotificationService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( VocabularyService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( LoggerService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( SecureService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( Platform, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( PluginService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( FileService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( ConfigService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( TimelineService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( TimepipeService, MENGINE_DOCUMENT_FACTORABLE );

        LOGGER_MESSAGE( "debug mode [%s]", Helper::isDebugMode() == true ? "ON" : "OFF" );
        LOGGER_MESSAGE( "development mode [%s]", Helper::isDevelopmentMode() == true ? "ON" : "OFF" );
        LOGGER_MESSAGE( "build publish [%s]", Helper::isBuildPublish() == true ? "ON" : "OFF" );
        LOGGER_MESSAGE( "build master [%s]", Helper::isBuildMaster() == true ? "ON" : "OFF" );

#if MENGINE_ASSERTION_DEBUG == 0
        LOGGER_MESSAGE( "enable assertion debug [OFF]" );
#endif

#if MENGINE_ASSERTION_DEBUG == 1
        LOGGER_MESSAGE( "enable assertion debug [ON]" );
#endif

#if MENGINE_LOGGER_DEBUG == 0
        LOGGER_MESSAGE( "enable logger debug [OFF]" );
#endif

#if MENGINE_LOGGER_DEBUG == 1
        LOGGER_MESSAGE( "enable logger debug [ON]" );
#endif

#if MENGINE_ALLOCATOR_DEBUG == 0
        LOGGER_MESSAGE( "enable allocator debug [OFF]" );
#endif

#if MENGINE_ALLOCATOR_DEBUG == 1
        LOGGER_MESSAGE( "enable allocator debug [ON]" );
#endif

#if MENGINE_DOCUMENT_ENABLE == 0
        LOGGER_MESSAGE( "enable document debug [OFF]" );
#endif

#if MENGINE_DOCUMENT_ENABLE == 1
        LOGGER_MESSAGE( "enable document debug [ON]" );
#endif

#ifdef MENGINE_PLUGIN_INI_STATIC
        MENGINE_ADD_PLUGIN( INI, "Plugin INI...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_JSON_STATIC
        MENGINE_ADD_PLUGIN( JSON, "Plugin JSON...", MENGINE_DOCUMENT_FACTORABLE );
#endif

        Char currentPath[MENGINE_MAX_PATH] = {'\0'};
        PLATFORM_SERVICE()
            ->getCurrentPath( currentPath );

        LOGGER_MESSAGE( "Current Folder: %s"
            , currentPath
        );

        LOGGER_INFO( "bootstrapper", "bootstrapper load application ini" );

        if( this->loadApplicationConfig_() == false )
        {
            LOGGER_ERROR( "invalid load application ini" );

            return false;
        }

        bool OPTION_assertion = HAS_OPTION( "assertion" );
        bool Engine_AssertionDebugBreak = CONFIG_VALUE( "Engine", "AssertionDebugBreak", false );

        bool Assertion_NoDebugBreak = OPTION_assertion == false && Engine_AssertionDebugBreak == false;

        Helper::AssertionSetNotDebugBreak( Assertion_NoDebugBreak );

        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_BASE_SERVICES );

#ifdef MENGINE_PLUGIN_OPTICK_STATIC
        MENGINE_ADD_PLUGIN( Optick, "initialize Optick...", MENGINE_DOCUMENT_FACTORABLE );
#endif

        LOGGER_INFO( "bootstrapper", "bootstrapper mount user file group" );

        if( this->mountUserFileGroup_() == false )
        {
            LOGGER_ERROR( "invalid mount user file group" );

            return false;
        }

        LOGGER_INFO( "bootstrapper", "bootstrapper initialize file logger" );

        if( this->initializeFileLogger_() == false )
        {
            LOGGER_ERROR( "invalid initalize file logger" );

            return false;
        }

#define BOOTSTRAPPER_SERVICE_CREATE( Name, Doc )\
        LOGGER_INFO("bootstrapper", "bootstrapper create service: " #Name);\
        SERVICE_CREATE(Name, Doc)\

        BOOTSTRAPPER_SERVICE_CREATE( SettingsService, MENGINE_DOCUMENT_FACTORABLE );

        BOOTSTRAPPER_SERVICE_CREATE( ThreadSystem, MENGINE_DOCUMENT_FACTORABLE );
        BOOTSTRAPPER_SERVICE_CREATE( ThreadService, MENGINE_DOCUMENT_FACTORABLE );

        BOOTSTRAPPER_SERVICE_CREATE( ArchiveService, MENGINE_DOCUMENT_FACTORABLE );

#ifdef MENGINE_PLUGIN_ZIP_STATIC
        MENGINE_ADD_PLUGIN( Zip, "Plugin Zip...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_LZ4_STATIC
        MENGINE_ADD_PLUGIN( LZ4, "Plugin LZ4...", MENGINE_DOCUMENT_FACTORABLE );
#endif

        bool OPTION_norenderMode = HAS_OPTION( "norender" );

        if( OPTION_norenderMode == true )
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

        bool OPTION_muteMode = HAS_OPTION( "mute" );

        if( OPTION_muteMode == true )
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
        BOOTSTRAPPER_SERVICE_CREATE( TimeSystem, MENGINE_DOCUMENT_FACTORABLE );
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
            LOGGER_ERROR( "invalid create dynamic priority dev plugins" );

            return false;
        }

#undef BOOTSTRAPPER_SERVICE_CREATE

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createStaticPlugins_()
    {
        LOGGER_MESSAGE( "Plugins..." );

#ifdef MENGINE_PLUGIN_SENTRY_STATIC
        MENGINE_ADD_PLUGIN( Sentry, "Plugin Sentry...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_EXTERNAL_SOURCE
        MENGINE_ADD_PLUGIN( ExternalBootstrapper, "initialize external Bootstrapper...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_PYTHONFRAMEWORK_STATIC
        MENGINE_ADD_PLUGIN( PythonFramework, "Plugin Python Framework...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_UIFRAMEWORK_STATIC
        MENGINE_ADD_PLUGIN( UIFramework, "Plugin UI Framework...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_NODEDEBUGRENDER_STATIC
        MENGINE_ADD_PLUGIN( NodeDebugRender, "Plugin Node Debug Render...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEPREFETCHER_STATIC
        MENGINE_ADD_PLUGIN( ResourcePrefetcher, "Plugin Resource Prefetcher...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_RESOURCECONVERT_STATIC
        MENGINE_ADD_PLUGIN( ResourceConvert, "Plugin Resource Convert...", MENGINE_DOCUMENT_FACTORABLE );
#endif        

#ifdef MENGINE_PLUGIN_RESOURCEVALIDATE_STATIC
        MENGINE_ADD_PLUGIN( ResourceValidate, "Plugin Resource Validate...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEDEBUGGER_STATIC
        MENGINE_ADD_PLUGIN( ResourceDebugger, "Plugin Resource Debugger...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_NODEDEBUGGER_STATIC
        MENGINE_ADD_PLUGIN( NodeDebugger, "Plugin NodeDebugger...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_OZZ_ANIMATION_STATIC
        MENGINE_ADD_PLUGIN( OzzAnimation, "Plugin OzzAnimation...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_BOX2D_STATIC
        MENGINE_ADD_PLUGIN( Box2D, "Plugin Box2D...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_TEXTUREPACKER_STATIC
        MENGINE_ADD_PLUGIN( Texturepacker, "Plugin Texturepacker...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_GRAPHICS_STATIC
        MENGINE_ADD_PLUGIN( Graphics, "Plugin Graphics...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_METABUFLOADER_STATIC
        MENGINE_ADD_PLUGIN( MetabufLoader, "Plugin Metabuf Loader...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_IMAGE_CODEC_STATIC
        MENGINE_ADD_PLUGIN( ImageCodec, "Plugin Image Codec...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_WAV_STATIC
        MENGINE_ADD_PLUGIN( WAV, "Plugin WAV Codec...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_OGG_VORBIS_STATIC
        MENGINE_ADD_PLUGIN( OggVorbis, "Plugin Ogg Vorbis Codec...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_AMPLIFIER_STATIC
        MENGINE_ADD_PLUGIN( Amplifier, "Plugin Amplifier...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_PVRTC_STATIC
        MENGINE_ADD_PLUGIN( PVRTC, "Plugin PVRTC...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_ETC1_STATIC
        MENGINE_ADD_PLUGIN( ETC1, "Plugin ETC1...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_DDS_STATIC
        MENGINE_ADD_PLUGIN( DDS, "Plugin DDS...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_JPEG_STATIC
        MENGINE_ADD_PLUGIN( JPEG, "Plugin JPEG...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_PNG_STATIC
        MENGINE_ADD_PLUGIN( PNG, "Plugin PNG...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_WEBP_STATIC
        MENGINE_ADD_PLUGIN( WebP, "Plugin WebP...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_VIDEO_STATIC
        MENGINE_ADD_PLUGIN( Video, "Plugin Video...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_THEORA_STATIC
        MENGINE_ADD_PLUGIN( Theora, "Plugin Theora...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_CURL_STATIC
        MENGINE_ADD_PLUGIN( cURL, "Plugin cURL...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_SPINE_STATIC
        MENGINE_ADD_PLUGIN( Spine, "Plugin Spine...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_MOVIE_STATIC
        MENGINE_ADD_PLUGIN( Movie, "Plugin Movie...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_MOVIE1_STATIC
        MENGINE_ADD_PLUGIN( Movie1, "Plugin Movie1...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_ASTRALAX_STATIC
        MENGINE_ADD_PLUGIN( Astralax, "Plugin Astralax...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_DAZZLE_STATIC
        MENGINE_ADD_PLUGIN( Dazzle, "Plugin Dazzle...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_BITMAPFONT_STATIC
        MENGINE_ADD_PLUGIN( BitmapFont, "Plugin Bitmap Font...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_TTF_STATIC
        MENGINE_ADD_PLUGIN( TTF, "Plugin TTF...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_FE_STATIC
        MENGINE_ADD_PLUGIN( FE, "Plugin FE...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_STEAM_STATIC
        MENGINE_ADD_PLUGIN( Steam, "Plugin Steam...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_WIN32_SOCKET_STATIC
        MENGINE_ADD_PLUGIN( Win32Socket, "Plugin Win32Socket...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_GOAP_STATIC
        MENGINE_ADD_PLUGIN( GOAP, "Plugin GOAP...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_DEVTODEBUG_STATIC
        MENGINE_ADD_PLUGIN( DevToDebug, "Plugin DevToDebug...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_IMGUI_STATIC
        MENGINE_ADD_PLUGIN( ImGUI, "Plugin ImGUI...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_CAMERADEBUGGIZMO_STATIC
        MENGINE_ADD_PLUGIN( CameraDebugGizmo, "Plugin Camera Debug Gizmo...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_WIN32_CONSOLELOGGER_STATIC
        MENGINE_ADD_PLUGIN( Win32ConsoleLogger, "Plugin Win32 Console Logger...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_WIN32_ANTIFREEZEMONITOR_STATIC
        MENGINE_ADD_PLUGIN( Win32AntifreezeMonitor, "Plugin Win32 Antifreeze Monitor...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_PYTHON_STATIC
        MENGINE_ADD_PLUGIN( AndroidNativePython, "Plugin Android Kernel Python...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_GAMEANALYTICS_STATIC
        MENGINE_ADD_PLUGIN( GameAnalytics, "Plugin Game Analytics...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_GAMECENTER_STATIC
        MENGINE_ADD_PLUGIN( AppleGameCenter, "Plugin Apple GameCenter...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_APPLE_APPTRACKING_STATIC
        MENGINE_ADD_PLUGIN( AppleAppTracking, "Plugin Apple AppTracking...", MENGINE_DOCUMENT_FACTORABLE );
#endif
        
#ifdef MENGINE_PLUGIN_APPLE_MARSDK_STATIC
        MENGINE_ADD_PLUGIN( MARSDK, "Plugin Apple MARSDK...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_XMLTOBIN_STATIC
        MENGINE_ADD_PLUGIN( XmlToBin, "Plugin XmlToBin...", MENGINE_DOCUMENT_FACTORABLE );
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
            if( PLUGIN_SERVICE()
                ->loadPlugin( pluginName.c_str(), MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "failed to load plugin '%s'"
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
        bool devplugins = MENGINE_MASTER_VALUE( false, Helper::isDevelopmentMode() );

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
                if( PLUGIN_SERVICE()
                    ->loadPlugin( pluginName.c_str(), MENGINE_DOCUMENT_FACTORABLE ) == false )
                {
                    LOGGER_WARNING( "failed to load dev plugin '%s'"
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
            if( PLUGIN_SERVICE()
                ->loadPlugin( pluginName.c_str(), MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "failed to load system plugin '%s'"
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
        bool devplugins = MENGINE_MASTER_VALUE( false, Helper::isDevelopmentMode() );

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
                if( PLUGIN_SERVICE()
                    ->loadPlugin( pluginName.c_str(), MENGINE_DOCUMENT_FACTORABLE ) == false )
                {
                    LOGGER_WARNING( "failed to load system dev plugin '%s'"
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
        bool devplugins = MENGINE_MASTER_VALUE( false, Helper::isDevelopmentMode() );

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
                if( PLUGIN_SERVICE()
                    ->loadPlugin( pluginName.c_str(), MENGINE_DOCUMENT_FACTORABLE ) == false )
                {
                    LOGGER_WARNING( "failed to load dev plugin '%s'"
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
        LOGGER_MESSAGE( "create Application..." );

        if( SERVICE_CREATE_SAFE( Application, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createFrameworks_()
    {
        LOGGER_MESSAGE( "create Frameworks..." );

        VectorConstString frameworks;
        CONFIG_VALUES( "Frameworks", "Name", &frameworks );

        for( const ConstString & name : frameworks )
        {
            if( FRAMEWORK_SERVICE()
                ->initializeFramework( name, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "failed to run framework '%s'"
                    , name.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::runModules_()
    {
        LOGGER_MESSAGE( "create Modules..." );

        VectorConstString modules;
        CONFIG_VALUES( "Modules", "Name", &modules );

        for( const ConstString & name : modules )
        {
            if( MODULE_SERVICE()
                ->runModule( name, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "failed to run module '%s'"
                    , name.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::runDevModules_()
    {
#ifdef MENGINE_MASTER_RELEASE
        bool devmodules = false;
#else
#ifdef MENGINE_DEBUG
        bool devmodules = true;
#else
        bool developmentMode = Helper::isDevelopmentMode();
        bool devmodules = developmentMode;
#endif
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

            for( const ConstString & name : devModules )
            {
                if( MODULE_SERVICE()
                    ->hasModule( name ) == false )
                {
                    LOGGER_ERROR( "not exist dev module '%s'"
                        , name.c_str()
                    );

                    continue;
                }

                if( MODULE_SERVICE()
                    ->runModule( name, MENGINE_DOCUMENT_FACTORABLE ) == false )
                {
                    LOGGER_ERROR( "failed to run dev module '%s'"
                        , name.c_str()
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
        VectorConstString modules;
        CONFIG_VALUES( "Modules", "Name", &modules );

        for( const ConstString & name : modules )
        {
            if( MODULE_SERVICE()
                ->hasModule( name ) == false )
            {
                LOGGER_ERROR( "not exist module '%s'"
                    , name.c_str()
                );

                continue;
            }

            if( MODULE_SERVICE()
                ->isRunModule( name ) == false )
            {
                LOGGER_ERROR( "not run module '%s'"
                    , name.c_str()
                );

                continue;
            }

            if( MODULE_SERVICE()
                ->stopModule( name ) == false )
            {
                LOGGER_ERROR( "failed to stop module '%s'"
                    , name.c_str()
                );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Bootstrapper::stopDevModules_()
    {
#ifdef MENGINE_MASTER_RELEASE
        bool devmodules = false;
#else
#ifdef MENGINE_DEBUG
        bool devmodules = true;
#else
        bool developmentMode = Helper::isDevelopmentMode();
        bool devmodules = developmentMode;
#endif
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

            for( const ConstString & name : devModules )
            {
                if( MODULE_SERVICE()
                    ->hasModule( name ) == false )
                {
                    LOGGER_ERROR( "not exist dev module '%s'"
                        , name.c_str()
                    );

                    continue;
                }

                if( MODULE_SERVICE()
                    ->isRunModule( name ) == false )
                {
                    LOGGER_ERROR( "not run dev module '%s'"
                        , name.c_str()
                    );

                    continue;
                }

                if( MODULE_SERVICE()
                    ->stopModule( name ) == false )
                {
                    LOGGER_ERROR( "failed to stop dev module '%s'"
                        , name.c_str()
                    );
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Bootstrapper::finalizeFrameworks_()
    {
        VectorConstString frameworks;
        CONFIG_VALUES( "Frameworks", "Name", &frameworks );

        for( const ConstString & name : frameworks )
        {
            if( FRAMEWORK_SERVICE()
                ->isInitializeFramework( name ) == false )
            {
                continue;
            }

            if( FRAMEWORK_SERVICE()
                ->finalizeFramework( name ) == false )
            {
                LOGGER_ERROR( "failed to stop framework '%s'"
                    , name.c_str()
                );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Bootstrapper::stopFrameworks_()
    {
        FRAMEWORK_SERVICE()
            ->stopFrameworks();
    }
    //////////////////////////////////////////////////////////////////////////
    void Bootstrapper::stop()
    {
        if( SERVICE_EXIST( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_ENGINE_STOP );
        }

        if( SERVICE_EXIST( GameServiceInterface ) == true )
        {
            GAME_SERVICE()
                ->interruption();
        }

        SERVICE_FINALIZE( SceneService );

        if( SERVICE_EXIST( GameServiceInterface ) == true )
        {
            GAME_SERVICE()
                ->stop();
        }

        m_packagesPaths.clear();
        m_settingsPaths.clear();

        if( SERVICE_EXIST( ApplicationInterface ) == true )
        {
            APPLICATION_SERVICE()
                ->finalizeGame();
        }

        if( SERVICE_EXIST( AccountServiceInterface ) == true )
        {
            ACCOUNT_SERVICE()
                ->stopAccounts();
        }

        if( SERVICE_EXIST( GameServiceInterface ) == true )
        {
            GAME_SERVICE()
                ->removePersonality();
        }

        if( SERVICE_EXIST( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_ENGINE_PREPARE_FINALIZE );
            NOTIFICATION_NOTIFY( NOTIFICATOR_ENGINE_FINALIZE );
        }

        this->stopModules_();
        this->stopDevModules_();

        SERVICE_PROVIDER_STOP();

        if( SERVICE_EXIST( FrameworkServiceInterface ) == true )
        {
            this->stopFrameworks_();

            this->finalizeFrameworks_();
        }

        if( SERVICE_EXIST( ThreadServiceInterface ) == true )
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

        if( SERVICE_EXIST( PluginServiceInterface ) == true )
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
        SERVICE_FINALIZE( SoundSystem );
        SERVICE_FINALIZE( ConverterService );
        SERVICE_FINALIZE( RenderService );
        SERVICE_FINALIZE( PrototypeService );
        SERVICE_FINALIZE( RenderMaterialService );
        SERVICE_FINALIZE( RenderTextureService );
        SERVICE_FINALIZE( RenderSystem );
        SERVICE_FINALIZE( ArchiveService );
        SERVICE_FINALIZE( MemoryService );
        SERVICE_FINALIZE( ThreadService );
        SERVICE_FINALIZE( TimeSystem );
        SERVICE_FINALIZE( EasingService );

        MENGINE_ASSERTION_VOCABULARY_EMPTY( STRINGIZE_STRING_LOCAL( "DebuggerBoundingBox" ) );

        SERVICE_FINALIZE( VocabularyService );
        SERVICE_FINALIZE( EnumeratorService );
        SERVICE_FINALIZE( PluginService );
        SERVICE_FINALIZE( TimepipeService );

        if( m_loggerFile != nullptr )
        {
            LOGGER_MESSAGE( "Finish log successful" );

            LOGGER_SERVICE()
                ->unregisterLogger( m_loggerFile );

            m_loggerFile = nullptr;
        }

        if( SERVICE_EXIST( FileServiceInterface ) == true )
        {
            FILE_SERVICE()
                ->unmountFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );
        }

        SERVICE_FINALIZE( UnicodeSystem );
        SERVICE_FINALIZE( FileService );
        SERVICE_FINALIZE( ThreadSystem );
        SERVICE_FINALIZE( Platform );
        SERVICE_FINALIZE( LoggerService );
        SERVICE_FINALIZE( NotificationService );
        SERVICE_FINALIZE( FactoryService );
        SERVICE_FINALIZE( OptionsService );

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
        SERVICE_DESTROY( EasingService );
        SERVICE_DESTROY( VocabularyService );
        SERVICE_DESTROY( EnumeratorService );
        SERVICE_DESTROY( PluginService );
        SERVICE_DESTROY( FileService );
        SERVICE_DESTROY( ThreadSystem );
        SERVICE_DESTROY( TimepipeService );
        SERVICE_DESTROY( Platform );
        SERVICE_DESTROY( NotificationService );
        SERVICE_DESTROY( LoggerService );
        SERVICE_DESTROY( FactoryService );
        SERVICE_DESTROY( OptionsService );
    }
    //////////////////////////////////////////////////////////////////////////
}
