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

#include "Kernel/Logger.h"
#include "Kernel/VectorConstString.h"
#include "Kernel/Document.h"

#include "Config/VectorString.h"

#ifndef MENGINE_SETLOCALE
#define MENGINE_SETLOCALE "C"
#endif

#ifndef MENGINE_APPLICATION_INI_PATH
#define MENGINE_APPLICATION_INI_PATH "application.ini"
#endif

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
SERVICE_EXTERN( ArchiveService );
SERVICE_EXTERN( ThreadService );
SERVICE_EXTERN( ThreadSystem );
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

PLUGIN_EXPORT( Zip );
PLUGIN_EXPORT( LZ4 );

PLUGIN_EXPORT( ImageCodec );
PLUGIN_EXPORT( SoundCodec );
PLUGIN_EXPORT( Amplifier );
PLUGIN_EXPORT( OggVorbis );

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
    bool Bootstrapper::run( const VectorFilePath & _packagesPaths )
    {
        if( this->createServices_() == false )
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
            ->initializeGame( defaultFileGroup, _packagesPaths ) == false )
        {
            LOGGER_CRITICAL( "Application invalid initialize game"
            );

            return false;
        }

        NOTIFICATION_NOTIFY( NOTIFICATOR_BOOTSTRAPPER_INITIALIZE_GAME );

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
    bool Bootstrapper::createServices_()
    {
        SERVICE_CREATE( OptionsService );
        SERVICE_CREATE( FactoryService );
        SERVICE_CREATE( SecureService );
        SERVICE_CREATE( MemoryService );
        SERVICE_CREATE( UnicodeSystem );        
        SERVICE_CREATE( NotificationService );
        SERVICE_CREATE( StringizeService );
        SERVICE_CREATE( VocabularyService );
        SERVICE_CREATE( LoggerService );
        SERVICE_CREATE( Platform );
        SERVICE_CREATE( PluginService );
        SERVICE_CREATE( FileService );
        SERVICE_CREATE( ConfigService );

        SERVICE_CREATE( ArchiveService );

        PLUGIN_CREATE( Zip );
        PLUGIN_CREATE( LZ4 );

        SERVICE_CREATE( ThreadSystem );
        SERVICE_CREATE( ThreadService );
        SERVICE_CREATE( PrototypeService );
        SERVICE_CREATE( RenderSystem );

        SERVICE_CREATE_SAFE( SoundSystem );

        bool muteMode = HAS_OPTION( "mute" );

        if( muteMode == true || SERVICE_EXIST( Mengine::SoundSystemInterface ) == false )
        {
            SERVICE_CREATE( SilentSoundSystem );
        }

        SERVICE_CREATE( SoundService );

        SERVICE_CREATE( ModuleService );
        SERVICE_CREATE( FrameworkService );
        SERVICE_CREATE( CodecService );
        SERVICE_CREATE( DataService );
        SERVICE_CREATE( PrefetcherService );
        SERVICE_CREATE( ConverterService );
        SERVICE_CREATE( InputService );
        SERVICE_CREATE( EnumeratorService );
        SERVICE_CREATE( ChronometerService );
        SERVICE_CREATE( TimeSystem );
        SERVICE_CREATE( EasingService );
        SERVICE_CREATE( UpdateService );
        SERVICE_CREATE( LoaderService );
        SERVICE_CREATE( RenderService );
        SERVICE_CREATE( RenderMaterialService );
        SERVICE_CREATE( RenderTextureService );
        SERVICE_CREATE( SceneService );
        SERVICE_CREATE( ResourceService );
        SERVICE_CREATE( TextService );
        SERVICE_CREATE( WatchdogService );
        SERVICE_CREATE( GraveyardService );
        SERVICE_CREATE( PackageService );
        SERVICE_CREATE( UserdataService );
        SERVICE_CREATE( PickerService );
        SERVICE_CREATE( PlayerService );
        SERVICE_CREATE( AccountService );
        SERVICE_CREATE( GameService );
        SERVICE_CREATE( TimelineService );
        SERVICE_CREATE( TimepipeService );
        SERVICE_CREATE( EnumeratorService );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createStaticPlugins_()
    {
        LOGGER_MESSAGE( "initialize Plugins..." );

#define MENGINE_ADD_PLUGIN( Name, Info )\
		{LOGGER_INFO( Info );\
		if(	PLUGIN_CREATE(Name) == false ){\
		LOGGER_ERROR( "Invalid %s", Info );}else{\
		LOGGER_MESSAGE( "Successful %s", Info );}}

#ifdef MENGINE_EXTERNAL_FRAMEWORK_STATIC
        MENGINE_ADD_PLUGIN( MENGINE_EXTERNAL_FRAMEWORK_NAME, "initialize Plugin External Framework..." );
#endif

#ifdef MENGINE_PLUGIN_PYTHONFRAMEWORK_STATIC
        MENGINE_ADD_PLUGIN( PythonFramework, "initialize Plugin Python Framework..." );
#endif

#ifdef MENGINE_PLUGIN_UIFRAMEWORK_STATIC
        MENGINE_ADD_PLUGIN( UIFramework, "initialize Plugin UI Framework..." );
#endif

#ifdef MENGINE_PLUGIN_NODEDEBUGRENDER_STATIC
        MENGINE_ADD_PLUGIN( NodeDebugRender, "initialize Plugin Node Debug Render..." );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEPREFETCHER_STATIC
        MENGINE_ADD_PLUGIN( ResourcePrefetcher, "initialize Plugin Resource Prefetcher..." );
#endif

#ifdef MENGINE_PLUGIN_RESOURCECONVERT_STATIC
        MENGINE_ADD_PLUGIN( ResourceConvert, "initialize Plugin Resource Convert..." );
#endif        

#ifdef MENGINE_PLUGIN_RESOURCEVALIDATE_STATIC
        MENGINE_ADD_PLUGIN( ResourceValidate, "initialize Plugin Resource Validate..." );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEDEBUGGER_STATIC
        MENGINE_ADD_PLUGIN( ResourceDebugger, "initialize Plugin Resource Debugger..." );
#endif

#ifdef MENGINE_PLUGIN_NODEDEBUGGER_STATIC
        MENGINE_ADD_PLUGIN( NodeDebugger, "initialize Plugin NodeDebugger..." );
#endif

#ifdef MENGINE_PLUGIN_OZZ_ANIMATION_STATIC
        MENGINE_ADD_PLUGIN( OzzAnimation, "initialize Plugin OzzAnimation..." );
#endif

#ifdef MENGINE_PLUGIN_JSON_STATIC
        MENGINE_ADD_PLUGIN( JSON, "initialize Plugin JSON..." );
#endif

#ifdef MENGINE_PLUGIN_BOX2D_STATIC
        MENGINE_ADD_PLUGIN( Box2D, "initialize Plugin Box2D..." );
#endif

#ifdef MENGINE_PLUGIN_TEXTUREPACKER_STATIC
        MENGINE_ADD_PLUGIN( Texturepacker, "initialize Plugin Texturepacker..." );
#endif

#ifdef MENGINE_PLUGIN_METABUFLOADER_STATIC
        MENGINE_ADD_PLUGIN( MetabufLoader, "initialize Plugin Metabuf Loader..." );
#endif

        MENGINE_ADD_PLUGIN( ImageCodec, "initialize Plugin Image Codec..." );
        MENGINE_ADD_PLUGIN( SoundCodec, "initialize Plugin Sound Codec..." );
        MENGINE_ADD_PLUGIN( OggVorbis, "initialize Plugin Ogg Vorbis Codec..." );
        MENGINE_ADD_PLUGIN( Amplifier, "initialize Plugin Amplifier..." );

#ifdef MENGINE_PLUGIN_WEBP_STATIC
        MENGINE_ADD_PLUGIN( WebP, "initialize Plugin WebP..." );
#endif

#ifdef MENGINE_PLUGIN_VIDEO_STATIC
        MENGINE_ADD_PLUGIN( Video, "initialize Plugin Video..." );
#endif

#ifdef MENGINE_PLUGIN_THEORA_STATIC
        MENGINE_ADD_PLUGIN( Theora, "initialize Plugin Theora..." );
#endif

#ifdef MENGINE_PLUGIN_CURL_STATIC
        MENGINE_ADD_PLUGIN( cURL, "initialize Plugin cURL..." );
#endif

#ifdef MENGINE_PLUGIN_SPINE_STATIC
        MENGINE_ADD_PLUGIN( Spine, "initialize Plugin Spine..." );
#endif

#ifdef MENGINE_PLUGIN_MOVIE_STATIC
        MENGINE_ADD_PLUGIN( Movie, "initialize Plugin Movie..." );
#endif

#ifdef MENGINE_PLUGIN_MOVIE1_STATIC
        MENGINE_ADD_PLUGIN( Movie1, "initialize Plugin Movie1..." );
#endif

#ifdef MENGINE_PLUGIN_BITMAPFONT_STATIC
        MENGINE_ADD_PLUGIN( BitmapFont, "initialize Plugin Bitmap Font..." );
#endif

#ifdef MENGINE_PLUGIN_TTF_STATIC
        MENGINE_ADD_PLUGIN( TTF, "initialize Plugin TTF..." );
#endif

#ifdef MENGINE_PLUGIN_WIN32_SOCKET_STATIC
        MENGINE_ADD_PLUGIN( Win32Socket, "initialize Plugin Win32Socket..." );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_FACEBOOK_STATIC
        MENGINE_ADD_PLUGIN( AndroidNativeFacebook, "initialize Android Facebook Native..." );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_UNITYADS_STATIC
        MENGINE_ADD_PLUGIN( AndroidNativeUnityAds, "initialize Android Unity Ads Native..." );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_ADMOB_STATIC
        MENGINE_ADD_PLUGIN( AndroidNativeAdMob, "initialize Android AdMob Native..." );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_DEVTODEV_STATIC
        MENGINE_ADD_PLUGIN( AndroidNativeDevToDev, "initialize Android DevToDev Native..." );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_LINKING_STATIC
        MENGINE_ADD_PLUGIN( AndroidNativeLinking, "initialize Android Linking Native..." );
#endif

#ifdef MENGINE_PLUGIN_ANDROID_NATIVE_LOCAL_NOTIFICATIONS_STATIC
        MENGINE_ADD_PLUGIN( AndroidNativeLocalNotifications, "initialize Android Local Notifications Native..." );
#endif

#	undef MENGINE_ADD_PLUGIN

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createDynamicPlugins_()
    {
        VectorString plugins;
        CONFIG_VALUES( "Plugins", "Name", plugins );

        for( const String & pluginName : plugins )
        {
            if( PLUGIN_SERVICE()
                ->loadPlugin( pluginName.c_str() ) == false )
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

        bool nodevplugins = HAS_OPTION( "nodevplugins" );

        if( devplugins == true && nodevplugins == false )
        {
            VectorString devPlugins;
            CONFIG_VALUES( "DevPlugins", "Name", devPlugins );

            for( const String & pluginName : devPlugins )
            {
                if( PLUGIN_SERVICE()
                    ->loadPlugin( pluginName.c_str() ) == false )
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

        SERVICE_CREATE( Application );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::createFrameworks_()
    {
        LOGGER_MESSAGE( "run Frameworks..." );

        VectorConstString frameworks;
        CONFIG_VALUES( "Frameworks", "Name", frameworks );

        for( const ConstString & name : frameworks )
        {
            if( FRAMEWORK_SERVICE()
                ->initializeFramework( name, MENGINE_DOCUMENT_FUNCTION ) == false )
            {
                LOGGER_CRITICAL( "failed to run framework '%s'"
                    , name.c_str()
                );
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Bootstrapper::runModules_()
    {
        LOGGER_MESSAGE( "run Modules..." );

        VectorConstString modules;
        CONFIG_VALUES( "Modules", "Name", modules );

        for( const ConstString & name : modules )
        {
            if( MODULE_SERVICE()
                ->runModule( name, MENGINE_DOCUMENT_FUNCTION ) == false )
            {
                LOGGER_CRITICAL( "failed to run module '%s'"
                    , name.c_str()
                );
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

        bool nodevmodules = HAS_OPTION( "nodevmodules" );

        if( devmodules == true && nodevmodules == false )
        {
            VectorConstString devModules;
            CONFIG_VALUES( "DevModules", "Name", devModules );

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
                    ->runModule( name, MENGINE_DOCUMENT_FUNCTION ) == false )
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
        CONFIG_VALUES( "Modules", "Name", modules );

        for( const ConstString & name : modules )
        {
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

        bool nodevmodules = HAS_OPTION( "nodevmodules" );

        if( devmodules == true && nodevmodules == false )
        {
            VectorConstString devModules;
            CONFIG_VALUES( "DevModules", "Name", devModules );

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
        CONFIG_VALUES( "Frameworks", "Name", frameworks );

        for( const ConstString & name : frameworks )
        {
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
        if( SERVICE_EXIST( PlatformInterface ) == true )
        {
            PLATFORM_SERVICE()
                ->stopPlatform();
        }

        ACCOUNT_SERVICE()
            ->stopAccounts();

        GAME_SERVICE()
            ->removePersonality();

        SCENE_SERVICE()
            ->destroyCurrentScene();

        //if( SERVICE_EXIST( FrameworkInterface ) == true )
        //{
        //    FRAMEWORK_SERVICE()
        //        ->onFrameworkFinalize();
        //}

        if( SERVICE_EXIST( NotificationServiceInterface ) == true )
        {
            NOTIFICATION_NOTIFY( NOTIFICATOR_ENGINE_FINALIZE );
        }

        SERVICE_PROVIDER_STOP();

        this->stopDevModules_();
        this->stopModules_();
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
        SERVICE_FINALIZE( DataService );

        PLUGIN_SERVICE()
            ->unloadPlugins();

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
        SERVICE_FINALIZE( FileService );
        SERVICE_FINALIZE( ThreadSystem );
        SERVICE_FINALIZE( TimepipeService );
        SERVICE_FINALIZE( Platform );
        SERVICE_FINALIZE( NotificationService );
        SERVICE_FINALIZE( LoggerService );
        SERVICE_FINALIZE( StringizeService );
        SERVICE_FINALIZE( FactoryService );
        SERVICE_FINALIZE( OptionsService );
    }
}
