#include "Bootstrapper.h"

#include "Interface/OptionsServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/ModuleServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/RenderMaterialServiceInterface.h"
#include "Interface/SoundSystemInterface.h"
#include "Interface/PlatformInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/FrameworkInterface.h"
#include "Interface/AccountServiceInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/GameServiceInterface.h"
#include "Interface/FrameworkServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/VectorConstString.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Stringalized.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/VectorString.h"
#include "Kernel/Stringstream.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/FileLogger.h"

#include <iomanip>

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_APPLICATION_INI_PATH
#define MENGINE_APPLICATION_INI_PATH "application.ini"
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_SETLOCALE
#define MENGINE_SETLOCALE "C"
#endif
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( SecureService );
SERVICE_EXTERN( FactoryService );
SERVICE_EXTERN( OptionsService );
SERVICE_EXTERN( StringizeService );
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
SERVICE_EXTERN( ComponentService );

PLUGIN_EXPORT( Zip );
PLUGIN_EXPORT( LZ4 );

PLUGIN_EXPORT( ImageCodec );
PLUGIN_EXPORT( SoundCodec );
PLUGIN_EXPORT( Amplifier );
PLUGIN_EXPORT( OggVorbis );

//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_EXTERNAL_SOURCE
PLUGIN_EXPORT( ExternalBootstrapper );
#endif

#ifdef MENGINE_PLUGIN_BITMAPFONT_STATIC
PLUGIN_EXPORT( BitmapFont );
#endif

#ifdef MENGINE_PLUGIN_NODEDEBUGRENDER_STATIC
PLUGIN_EXPORT( NodeDebugRender );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEPREFETCHER_STATIC
PLUGIN_EXPORT( ResourcePrefetcher );
#endif

#ifdef MENGINE_PLUGIN_RESOURCECONVERT_STATIC
PLUGIN_EXPORT( ResourceConvert );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEVALIDATE_STATIC
PLUGIN_EXPORT( ResourceValidate );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEDEBUGGER_STATIC
PLUGIN_EXPORT( ResourceDebugger );
#endif

#ifdef MENGINE_PLUGIN_METABUFLOADER_STATIC
PLUGIN_EXPORT( MetabufLoader );
#endif

#ifdef MENGINE_PLUGIN_WEBP_STATIC
PLUGIN_EXPORT( WebP );
#endif

#ifdef MENGINE_PLUGIN_VIDEO_STATIC
PLUGIN_EXPORT( Video );
#endif

#ifdef MENGINE_PLUGIN_THEORA_STATIC
PLUGIN_EXPORT( Theora );
#endif

#ifdef MENGINE_PLUGIN_CURL_STATIC
PLUGIN_EXPORT( cURL );
#endif

#ifdef MENGINE_PLUGIN_SPINE_STATIC
PLUGIN_EXPORT( Spine );
#endif

#ifdef MENGINE_PLUGIN_MOVIE_STATIC
PLUGIN_EXPORT( Movie );
#endif

#ifdef MENGINE_PLUGIN_MOVIE1_STATIC
PLUGIN_EXPORT( Movie1 );
#endif

#ifdef MENGINE_PLUGIN_TTF_STATIC
PLUGIN_EXPORT( TTF );
#endif

#ifdef MENGINE_PLUGIN_WIN32_SOCKET_STATIC
PLUGIN_EXPORT( Win32Socket );
#endif

#ifdef MENGINE_PLUGIN_NODEDEBUGGER_STATIC
PLUGIN_EXPORT( NodeDebugger );
#endif

#ifdef MENGINE_PLUGIN_OZZ_ANIMATION_STATIC
PLUGIN_EXPORT( OzzAnimation );
#endif

#ifdef MENGINE_EXTERNAL_FRAMEWORK_STATIC
PLUGIN_EXPORT( MENGINE_EXTERNAL_FRAMEWORK_NAME );
#endif

#ifdef MENGINE_PLUGIN_PYTHONFRAMEWORK_STATIC
PLUGIN_EXPORT( PythonFramework );
#endif

#ifdef MENGINE_PLUGIN_UIFRAMEWORK_STATIC
PLUGIN_EXPORT( UIFramework );
#endif

#ifdef MENGINE_PLUGIN_JSON_STATIC
PLUGIN_EXPORT( JSON );
#endif

#ifdef MENGINE_PLUGIN_BOX2D_STATIC
PLUGIN_EXPORT( Box2D );
#endif

#ifdef MENGINE_PLUGIN_TEXTUREPACKER_STATIC
PLUGIN_EXPORT( Texturepacker );
#endif

#ifdef MENGINE_PLUGIN_GRAPHICS_STATIC
PLUGIN_EXPORT( Graphics );
#endif

#ifdef MENGINE_PLUGIN_SENTRY_STATIC
PLUGIN_EXPORT( Sentry );
#endif

#ifdef MENGINE_PLUGIN_GOAP_STATIC
PLUGIN_EXPORT( GOAP );
#endif

#ifdef MENGINE_PLUGIN_IMGUI_STATIC
PLUGIN_EXPORT( ImGUI );
#endif

#ifdef MENGINE_PLUGIN_CAMERADEBUGGIZMO_STATIC
PLUGIN_EXPORT( CameraDebugGizmo );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_KERNEL_STATIC
PLUGIN_EXPORT( AndroidNativeKernel );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_FACEBOOK_STATIC
PLUGIN_EXPORT( AndroidNativeFacebook );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_UNITYADS_STATIC
PLUGIN_EXPORT( AndroidNativeUnityAds );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_ADMOB_STATIC
PLUGIN_EXPORT( AndroidNativeAdMob );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_DEVTODEV_STATIC
PLUGIN_EXPORT( AndroidNativeDevToDev );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_LINKING_STATIC
PLUGIN_EXPORT( AndroidNativeLinking );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_LOCAL_NOTIFICATIONS_STATIC
PLUGIN_EXPORT( AndroidNativeLocalNotifications );
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
            return false;
        }

        if( this->createDynamicPriorityPlugins_() == false )
        {
            return false;
        }

        if( this->createDynamicPriorityDevPlugins_() == false )
        {
            return false;
        }

        if( this->createStaticPlugins_() == false )
        {
            return false;
        }

        if( this->createDynamicPlugins_() == false )
        {
            return false;
        }

        if( this->createDynamicDevPlugins_() == false )
        {
            return false;
        }

        if( this->createApplication_() == false )
        {
            return false;
        }

        if( this->runModules_() == false )
        {
            return false;
        }

        if( this->runDevModules_() == false )
        {
            return false;
        }

        if( this->createFrameworks_() == false )
        {
            return false;
        }

        if( this->runFrameworks_() == false )
        {
            return false;
        }
        
        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_RUN_FRAMEWORKS );

        LOGGER_INFO( "initialize Game..." );

        const FileGroupInterfacePtr & defaultFileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        if( APPLICATION_SERVICE()
            ->initializeGame( defaultFileGroup, m_packagesPaths, m_settingsPaths ) == false )
        {
            LOGGER_CRITICAL( "Application invalid initialize game"
            );

            return false;
        }

        if( GAME_SERVICE()
            ->loadPersonality() == false )
        {
            LOGGER_CRITICAL( "Game invalid load personality"
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::loadApplicationIni_()
    {
        FilePath applicationPath = Helper::stringizeFilePath( MENGINE_APPLICATION_INI_PATH );

        const FileGroupInterfacePtr & defaultFileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        if( defaultFileGroup->existFile( applicationPath, true ) == true )
        {
            ConfigInterfacePtr applicationConfig = CONFIG_SERVICE()
                ->loadConfig( defaultFileGroup, applicationPath, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( applicationConfig, "invalid open application settings '%s'"
                , applicationPath.c_str()
            );

            VectorFilePath configsPaths;
            applicationConfig->getValues( "Configs", "Path", &configsPaths );

            for( const FilePath & filePath : configsPaths )
            {
                if( CONFIG_SERVICE()
                    ->loadDefaultConfig( defaultFileGroup, filePath, MENGINE_DOCUMENT_FACTORABLE ) == false )
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
                    ->loadDefaultConfig( defaultFileGroup, filePath, MENGINE_DOCUMENT_FACTORABLE ) == false )
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
            LOGGER_INFO( "not exist application config '%s'"
                , applicationPath.c_str()
            );
        }

        const Char * option_config = GET_OPTION_VALUE( "config", nullptr );

        if( option_config != nullptr )
        {
            if( CONFIG_SERVICE()
                ->loadDefaultConfig( defaultFileGroup, Helper::stringizeFilePath( option_config ), MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_ERROR( "invalid load config '%s'"
                    , option_config
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::mountUserFileGroup_()
    {
        Char userPath[MENGINE_MAX_PATH] = {0};
        size_t userPathLen = PLATFORM_SERVICE()
            ->getUserPath( userPath );

        FilePath cs_userPath = Helper::stringizeFilePathSize( userPath, (FilePath::size_type)userPathLen );

        LOGGER_MESSAGE( "mount user file group: %s"
            , cs_userPath.c_str()
        );

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
        bool nologs = HAS_OPTION( "nologs" );

        if( nologs == true )
        {
            return true;
        }

        DateTimeProviderInterfacePtr dateTimeProvider = PLATFORM_SERVICE()
            ->createDateTimeProvider( MENGINE_DOCUMENT_FACTORABLE );

        PlatformDateTime dateTime;
        dateTimeProvider->getLocalDateTime( &dateTime );

        Stringstream ss_date;
        ss_date << dateTime.year
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << (dateTime.month)
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.day
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.hour
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.minute
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.second;

        String str_date = ss_date.str();

        WString unicode_date;
        Helper::utf8ToUnicode( str_date, &unicode_date );

        WString unicode_logFilename;
        unicode_logFilename += L"Game";

        bool developmentMode = HAS_OPTION( "dev" );
        bool roamingMode = HAS_OPTION( "roaming" );
        bool noroamingMode = HAS_OPTION( "noroaming" );

        if( developmentMode == true && (roamingMode == false || noroamingMode == false) )
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

            LOGGER_INFO( "write log to '%s'"
                , logFilename.c_str()
            );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createServices_()
    {
        SERVICE_CREATE( EnumeratorService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( OptionsService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( FactoryService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( SecureService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( MemoryService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( UnicodeSystem, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( NotificationService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( StringizeService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( VocabularyService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( LoggerService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( Platform, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( PluginService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( FileService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( ConfigService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( TimelineService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( TimepipeService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( ComponentService, MENGINE_DOCUMENT_FACTORABLE );

        if( this->loadApplicationIni_() == false )
        {
            return false;
        }

        if( this->mountUserFileGroup_() == false )
        {
            return false;
        }

        if( this->initializeFileLogger_() == false )
        {
            return false;
        }

        SERVICE_CREATE( SettingsService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( ArchiveService, MENGINE_DOCUMENT_FACTORABLE );

        PLUGIN_CREATE( Zip, MENGINE_DOCUMENT_FACTORABLE );
        PLUGIN_CREATE( LZ4, MENGINE_DOCUMENT_FACTORABLE );

        SERVICE_CREATE( ThreadSystem, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( ThreadService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( PrototypeService, MENGINE_DOCUMENT_FACTORABLE );

        bool norenderMode = HAS_OPTION( "norender" );

        if( norenderMode == true )
        {
            SERVICE_CREATE( MockupRenderSystem, MENGINE_DOCUMENT_FACTORABLE );
        }
        else
        {
            SERVICE_CREATE( RenderSystem, MENGINE_DOCUMENT_FACTORABLE );
        }

        SERVICE_CREATE( RenderService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( RenderMaterialService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( RenderTextureService, MENGINE_DOCUMENT_FACTORABLE );

        bool muteMode = HAS_OPTION( "mute" );

        if( muteMode == true )
        {
            SERVICE_CREATE( SilentSoundSystem, MENGINE_DOCUMENT_FACTORABLE );
        }
        else
        {
            if( SERVICE_CREATE_SAFE( SoundSystem, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                SERVICE_CREATE( SilentSoundSystem, MENGINE_DOCUMENT_FACTORABLE );
            }
        }

        SERVICE_CREATE( SoundService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( ModuleService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( FrameworkService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( CodecService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( DataService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( PrefetcherService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( ConverterService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( InputService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( ChronometerService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( TimeSystem, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( EasingService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( UpdateService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( LoaderService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( SceneService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( ResourceService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( TextService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( WatchdogService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( GraveyardService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( PackageService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( UserdataService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( PickerService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( PlayerService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( AccountService, MENGINE_DOCUMENT_FACTORABLE );
        SERVICE_CREATE( GameService, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createStaticPlugins_()
    {
        LOGGER_MESSAGE( "initialize Plugins..." );

#define MENGINE_ADD_PLUGIN( Name, Info, Doc )\
        {LOGGER_INFO( Info );\
        if( PLUGIN_CREATE(Name, Doc) == false ){\
        LOGGER_ERROR( "Invalid %s", Info ); return false;}else{\
        LOGGER_MESSAGE( "Successful %s", Info );}}

#ifdef MENGINE_PLUGIN_SENTRY_STATIC
        MENGINE_ADD_PLUGIN( Sentry, "initialize Plugin Sentry...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_EXTERNAL_SOURCE
        MENGINE_ADD_PLUGIN( ExternalBootstrapper, "initialize external Bootstrapper...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_EXTERNAL_FRAMEWORK_STATIC
        MENGINE_ADD_PLUGIN( MENGINE_EXTERNAL_FRAMEWORK_NAME, "initialize Plugin External Framework...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_PYTHONFRAMEWORK_STATIC
        MENGINE_ADD_PLUGIN( PythonFramework, "initialize Plugin Python Framework...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_UIFRAMEWORK_STATIC
        MENGINE_ADD_PLUGIN( UIFramework, "initialize Plugin UI Framework...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_NODEDEBUGRENDER_STATIC
        MENGINE_ADD_PLUGIN( NodeDebugRender, "initialize Plugin Node Debug Render...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEPREFETCHER_STATIC
        MENGINE_ADD_PLUGIN( ResourcePrefetcher, "initialize Plugin Resource Prefetcher...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_RESOURCECONVERT_STATIC
        MENGINE_ADD_PLUGIN( ResourceConvert, "initialize Plugin Resource Convert...", MENGINE_DOCUMENT_FACTORABLE );
#endif        

#ifdef MENGINE_PLUGIN_RESOURCEVALIDATE_STATIC
        MENGINE_ADD_PLUGIN( ResourceValidate, "initialize Plugin Resource Validate...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEDEBUGGER_STATIC
        MENGINE_ADD_PLUGIN( ResourceDebugger, "initialize Plugin Resource Debugger...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_NODEDEBUGGER_STATIC
        MENGINE_ADD_PLUGIN( NodeDebugger, "initialize Plugin NodeDebugger...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_OZZ_ANIMATION_STATIC
        MENGINE_ADD_PLUGIN( OzzAnimation, "initialize Plugin OzzAnimation...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_JSON_STATIC
        MENGINE_ADD_PLUGIN( JSON, "initialize Plugin JSON...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_BOX2D_STATIC
        MENGINE_ADD_PLUGIN( Box2D, "initialize Plugin Box2D...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_TEXTUREPACKER_STATIC
        MENGINE_ADD_PLUGIN( Texturepacker, "initialize Plugin Texturepacker...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_GRAPHICS_STATIC
        MENGINE_ADD_PLUGIN( Graphics, "initialize Plugin Graphics...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_METABUFLOADER_STATIC
        MENGINE_ADD_PLUGIN( MetabufLoader, "initialize Plugin Metabuf Loader...", MENGINE_DOCUMENT_FACTORABLE );
#endif

        MENGINE_ADD_PLUGIN( ImageCodec, "initialize Plugin Image Codec...", MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_PLUGIN( SoundCodec, "initialize Plugin Sound Codec...", MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_PLUGIN( OggVorbis, "initialize Plugin Ogg Vorbis Codec...", MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ADD_PLUGIN( Amplifier, "initialize Plugin Amplifier...", MENGINE_DOCUMENT_FACTORABLE );

#ifdef MENGINE_PLUGIN_WEBP_STATIC
        MENGINE_ADD_PLUGIN( WebP, "initialize Plugin WebP...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_VIDEO_STATIC
        MENGINE_ADD_PLUGIN( Video, "initialize Plugin Video...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_THEORA_STATIC
        MENGINE_ADD_PLUGIN( Theora, "initialize Plugin Theora...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_CURL_STATIC
        MENGINE_ADD_PLUGIN( cURL, "initialize Plugin cURL...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_SPINE_STATIC
        MENGINE_ADD_PLUGIN( Spine, "initialize Plugin Spine...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_MOVIE_STATIC
        MENGINE_ADD_PLUGIN( Movie, "initialize Plugin Movie...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_MOVIE1_STATIC
        MENGINE_ADD_PLUGIN( Movie1, "initialize Plugin Movie1...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_BITMAPFONT_STATIC
        MENGINE_ADD_PLUGIN( BitmapFont, "initialize Plugin Bitmap Font...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_TTF_STATIC
        MENGINE_ADD_PLUGIN( TTF, "initialize Plugin TTF...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_WIN32_SOCKET_STATIC
        MENGINE_ADD_PLUGIN( Win32Socket, "initialize Plugin Win32Socket...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_GOAP_STATIC
        MENGINE_ADD_PLUGIN( GOAP, "initialize Plugin GOAP...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_IMGUI_STATIC
        MENGINE_ADD_PLUGIN( ImGUI, "initialize Plugin ImGUI...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_CAMERADEBUGGIZMO
        MENGINE_ADD_PLUGIN( CameraDebugGizmo, "initialize Plugin CameraDebugGizmo...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_KERNEL_STATIC
        MENGINE_ADD_PLUGIN( AndroidNativeKernel, "initialize Android Kernel Native...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_FACEBOOK_STATIC
        MENGINE_ADD_PLUGIN( AndroidNativeFacebook, "initialize Android Facebook Native...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_UNITYADS_STATIC
        MENGINE_ADD_PLUGIN( AndroidNativeUnityAds, "initialize Android Unity Ads Native...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_ADMOB_STATIC
        MENGINE_ADD_PLUGIN( AndroidNativeAdMob, "initialize Android AdMob Native...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_DEVTODEV_STATIC
        MENGINE_ADD_PLUGIN( AndroidNativeDevToDev, "initialize Android DevToDev Native...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_LINKING_STATIC
        MENGINE_ADD_PLUGIN( AndroidNativeLinking, "initialize Android Linking Native...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_LOCAL_NOTIFICATIONS_STATIC
        MENGINE_ADD_PLUGIN( AndroidNativeLocalNotifications, "initialize Android Local Notifications Native...", MENGINE_DOCUMENT_FACTORABLE );
#endif

#undef MENGINE_ADD_PLUGIN

        return true;
    }
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
                LOGGER_CRITICAL( "failed to load plugin '%s'"
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
#ifdef MENGINE_MASTER_RELEASE
        bool devplugins = false;
#else
#ifdef MENGINE_DEBUG
        bool devplugins = true;
#else
        bool developmentMode = HAS_OPTION( "dev" );
        bool devplugins = developmentMode;
#endif
#endif

        if( HAS_OPTION( "devplugins" ) == true )
        {
            devplugins = true;
        }

        bool nodevplugins = HAS_OPTION( "nodevplugins" );

        if( devplugins == true && nodevplugins == false )
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
    bool Bootstrapper::createDynamicPriorityPlugins_()
    {
        VectorString plugins;
        CONFIG_VALUES( "PriorityPlugins", "Name", &plugins );

        for( const String & pluginName : plugins )
        {
            if( PLUGIN_SERVICE()
                ->loadPlugin( pluginName.c_str(), MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_CRITICAL( "failed to load plugin '%s'"
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
#ifdef MENGINE_MASTER_RELEASE
        bool devplugins = false;
#else
#ifdef MENGINE_DEBUG
        bool devplugins = true;
#else
        bool developmentMode = HAS_OPTION( "dev" );
        bool devplugins = developmentMode;
#endif
#endif

        if( HAS_OPTION( "devplugins" ) == true )
        {
            devplugins = true;
        }

        bool nodevplugins = HAS_OPTION( "nodevplugins" );

        if( devplugins == true && nodevplugins == false )
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

        SERVICE_CREATE( Application, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_CREATE_APPLICATION );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createFrameworks_()
    {
        LOGGER_MESSAGE( "run Frameworks..." );

        VectorConstString frameworks;
        CONFIG_VALUES( "Frameworks", "Name", &frameworks );

        for( const ConstString & name : frameworks )
        {
            if( FRAMEWORK_SERVICE()
                ->initializeFramework( name, MENGINE_DOCUMENT_FACTORABLE ) == false )
            {
                LOGGER_CRITICAL( "failed to run framework '%s'"
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
        LOGGER_MESSAGE( "run Modules..." );

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
        bool developmentMode = HAS_OPTION( "dev" );
        bool devmodules = developmentMode;
#endif
#endif

        if( HAS_OPTION( "devmodules" ) == true )
        {
            devmodules = true;
        }

        bool nodevmodules = HAS_OPTION( "nodevmodules" );

        if( devmodules == true && nodevmodules == false )
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
                LOGGER_CRITICAL( "failed to stop module '%s'"
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
        bool developmentMode = HAS_OPTION( "dev" );
        bool devmodules = developmentMode;
#endif
#endif

        if( HAS_OPTION( "devmodules" ) == true )
        {
            devmodules = true;
        }

        bool nodevmodules = HAS_OPTION( "nodevmodules" );

        if( devmodules == true && nodevmodules == false )
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
                LOGGER_CRITICAL( "failed to stop framework '%s'"
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
        m_packagesPaths.clear();
        m_settingsPaths.clear();

        SCENE_SERVICE()
            ->destroyCurrentScene();

        if( SERVICE_EXIST( ApplicationInterface ) == true )
        {
            APPLICATION_SERVICE()
                ->finalizeGame();
        }

        if( SERVICE_EXIST( PlatformInterface ) == true )
        {
            PLATFORM_SERVICE()
                ->stopPlatform();
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
            NOTIFICATION_NOTIFY( NOTIFICATOR_ENGINE_FINALIZE );
        }

        if( SERVICE_EXIST( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_SCRIPT_EJECTING );
        }

        this->stopModules_();
        this->stopDevModules_();

        SERVICE_PROVIDER_STOP();

        this->stopFrameworks_();

        this->finalizeFrameworks_();

        if( SERVICE_EXIST( ThreadServiceInterface ) == true )
        {
            THREAD_SERVICE()
                ->stopTasks();
        }

        SERVICE_FINALIZE( SceneService );
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
        SERVICE_FINALIZE( SettingsService );
        SERVICE_FINALIZE( DataService );

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
        SERVICE_FINALIZE( UnicodeSystem );
        SERVICE_FINALIZE( CodecService );
        SERVICE_FINALIZE( SoundService );
        SERVICE_FINALIZE( SoundSystem );
        SERVICE_FINALIZE( ConverterService );
        SERVICE_FINALIZE( RenderService );
        SERVICE_FINALIZE( PrototypeService );
        SERVICE_FINALIZE( RenderMaterialService );
        SERVICE_FINALIZE( RenderTextureService );
        SERVICE_FINALIZE( RenderSystem );
        SERVICE_FINALIZE( ConfigService );
        SERVICE_FINALIZE( ArchiveService );
        SERVICE_FINALIZE( MemoryService );
        SERVICE_FINALIZE( ThreadService );
        SERVICE_FINALIZE( TimeSystem );
        SERVICE_FINALIZE( EasingService );
        SERVICE_FINALIZE( VocabularyService );
        SERVICE_FINALIZE( EnumeratorService );
        SERVICE_FINALIZE( PluginService );
        SERVICE_FINALIZE( TimepipeService );
        SERVICE_FINALIZE( ComponentService );

        if( m_loggerFile != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_loggerFile );

            m_loggerFile = nullptr;
        }

        SERVICE_FINALIZE( FileService );
        SERVICE_FINALIZE( ThreadSystem );
        SERVICE_FINALIZE( Platform );
        SERVICE_FINALIZE( NotificationService );
        SERVICE_FINALIZE( LoggerService );
        SERVICE_FINALIZE( StringizeService );
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
        SERVICE_DESTROY( ComponentService );
        SERVICE_DESTROY( Platform );
        SERVICE_DESTROY( NotificationService );
        SERVICE_DESTROY( LoggerService );
        SERVICE_DESTROY( StringizeService );
        SERVICE_DESTROY( FactoryService );
        SERVICE_DESTROY( OptionsService );
    }
}