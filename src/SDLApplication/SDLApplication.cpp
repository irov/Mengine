#include "SDLApplication.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/BootstrapperInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PersistentSystemInterface.h"

#include "SDLMessageBoxLogger.h"

#if defined(MENGINE_PLATFORM_ANDROID)
#   include "Environment/Android/AndroidEnv.h"
#elif defined(MENGINE_PLATFORM_APPLE)
#   include "Environment/Apple/AppleUtils.h"
#endif

#if defined(MENGINE_PLATFORM_ANDROID)
#   include "Environment/Android/AndroidLogger.h"
#elif defined(MENGINE_PLATFORM_APPLE)
#   include "Environment/Apple/AppleNSLogger.h"
#else
#   include "Kernel/StdioLogger.h"
#endif

#if defined(MENGINE_WINDOWS_DEBUG)
#   include "Environment/Windows/Win32OutputDebugLogger.h"
#endif

#include "Kernel/ConfigHelper.h"
#include "Kernel/StringArguments.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/Error.h"

#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_MENGINE_STATIC)
extern Mengine::ServiceProviderInterface * initializeMengine();
extern bool bootstrapMengine();
extern void finalizeMengine();
#endif
//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLATFORM_ANDROID)
        //////////////////////////////////////////////////////////////////////////
        static bool addAndroidBuildConfigOptions( JNIEnv * _jenv, const Char * _buildConfig, const ArgumentsInterfacePtr & _arguments )
        {
            jclass jclass_BuildConfig = _jenv->FindClass( "org/Mengine/Project/BuildConfig" );

            if( jclass_BuildConfig == nullptr )
            {
                return false;
            }

            jfieldID jfield_MENGINE_APP_OPTIONS = _jenv->GetStaticFieldID( jclass_BuildConfig, _buildConfig, "Ljava/lang/String;" );

            if( _jenv->ExceptionCheck() == true )
            {
                _jenv->ExceptionClear();

                _jenv->DeleteLocalRef( jclass_BuildConfig );

                return false;
            }

            if( jfield_MENGINE_APP_OPTIONS == nullptr )
            {
                _jenv->DeleteLocalRef( jclass_BuildConfig );

                return false;
            }

            jstring j_MENGINE_APP_OPTIONS = (jstring)_jenv->GetStaticObjectField( jclass_BuildConfig, jfield_MENGINE_APP_OPTIONS );

            if( j_MENGINE_APP_OPTIONS == nullptr )
            {
                _jenv->DeleteLocalRef( jclass_BuildConfig );

                return false;
            }

            const Char * MENGINE_APP_OPTIONS_str = _jenv->GetStringUTFChars( j_MENGINE_APP_OPTIONS, nullptr );

            _arguments->addArguments( MENGINE_APP_OPTIONS_str );

            _jenv->ReleaseStringUTFChars( j_MENGINE_APP_OPTIONS, MENGINE_APP_OPTIONS_str );
            _jenv->DeleteLocalRef( j_MENGINE_APP_OPTIONS );
            _jenv->DeleteLocalRef( jclass_BuildConfig );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
#endif
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    SDLApplication::SDLApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLApplication::~SDLApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeOptionsService_( int32_t _argc, Char ** const _argv )
    {
        ArgumentsInterfacePtr arguments = Helper::makeFactorableUnique<StringArguments>( MENGINE_DOCUMENT_FUNCTION );

#if defined(MENGINE_PLATFORM_ANDROID)
        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Detail::addAndroidBuildConfigOptions( jenv, "MENGINE_APP_OPTIONS", arguments );
#endif


#if defined(MENGINE_PLATFORM_APPLE) && !defined(MENGINE_BUILD_PUBLISH)
        Char MengineApplePersistentArguments[1024] = {'\0'};
        if( PERSISTENT_SYSTEM()
            ->getPersistentArguments( MengineApplePersistentArguments, 1024 ) == true )
        {
            arguments->addArguments( MengineApplePersistentArguments );
        }
#endif

        for( int32_t i = 1; i < _argc; ++i )
        {
            const Char * arg = _argv[i];

            arguments->addArgument( arg );
        }

        if( OPTIONS_SERVICE()
            ->setArguments( arguments ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeLoggerService_()
    {
        if( LOGGER_SERVICE()
            ->isSilent() == true )
        {
            return true;
        }

#if defined(MENGINE_PLATFORM_ANDROID)
        LoggerInterfacePtr loggerStdio = Helper::makeFactorableUnique<AndroidLogger>( MENGINE_DOCUMENT_FUNCTION );
#elif defined(MENGINE_PLATFORM_APPLE)
        LoggerInterfacePtr loggerStdio = Helper::makeFactorableUnique<AppleNSLogger>( MENGINE_DOCUMENT_FUNCTION );
#else
        LoggerInterfacePtr loggerStdio = Helper::makeFactorableUnique<StdioLogger>( MENGINE_DOCUMENT_FUNCTION );
#endif

        loggerStdio->setWriteHistory( true );

        LOGGER_SERVICE()
            ->registerLogger( loggerStdio );

        m_loggerStdio = loggerStdio;

        LoggerInterfacePtr loggerMessageBox = Helper::makeFactorableUnique<SDLMessageBoxLogger>( MENGINE_DOCUMENT_FUNCTION );

        loggerMessageBox->setVerboseLevel( LM_FATAL );

        LOGGER_SERVICE()
            ->registerLogger( loggerMessageBox );

        m_loggerMessageBox = loggerMessageBox;

#if defined(MENGINE_WINDOWS_DEBUG)
        Win32OutputDebugLoggerPtr loggerOutputDebug = Helper::makeFactorableUnique<Win32OutputDebugLogger>( MENGINE_DOCUMENT_FUNCTION );

        loggerOutputDebug->setVerboseLevel( LM_MESSAGE );

        LOGGER_SERVICE()
            ->registerLogger( loggerOutputDebug );

        m_loggerOutputDebug = loggerOutputDebug;
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLApplication::finalizeLoggerService_()
    {
        if( m_loggerStdio != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_loggerStdio );

            m_loggerStdio = nullptr;
        }

        if( m_loggerMessageBox != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_loggerMessageBox );

            m_loggerMessageBox = nullptr;
        }

#if defined(MENGINE_WINDOWS_DEBUG)
        if( m_loggerOutputDebug != nullptr )
        {
            if( SERVICE_IS_INITIALIZE( LoggerServiceInterface ) == true )
            {
                LOGGER_SERVICE()
                    ->unregisterLogger( m_loggerOutputDebug );
            }

            m_loggerOutputDebug = nullptr;
        }
#endif
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::bootstrap( int32_t _argc, Char ** const _argv )
    {
#if defined(MENGINE_PLUGIN_MENGINE_DLL)
#   error "MENGINE_PLUGIN_MENGINE_DLL for SDL not implemented"
#elif defined(MENGINE_PLUGIN_MENGINE_STATIC)
        ServiceProviderInterface * serviceProvider = initializeMengine();
#else
        ServiceProviderInterface * serviceProvider = nullptr;
#endif

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_PROVIDER_GET()->waitService( "SDLApplication", SERVICE_ID( OptionsServiceInterface ), [this, _argc, _argv]()
        {
            if( this->initializeOptionsService_( _argc, _argv ) == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_WAIT( SDLApplication, LoggerServiceInterface, [this]()
        {
            if( this->initializeLoggerService_() == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_LEAVE( SDLApplication, LoggerServiceInterface, [this]()
        {
            this->finalizeLoggerService_();
        } );

#if defined(MENGINE_PLUGIN_MENGINE_DLL)
#error "MENGINE_PLUGIN_MENGINE_DLL for SDL not implemented"
#elif defined(MENGINE_PLUGIN_MENGINE_STATIC)
        if( bootstrapMengine() == false )
        {
            return false;
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initialize()
    {
        LOGGER_INFO( "application", "creating render window..." );

        const Char * projectTitle = nullptr;
        size_t projectTitleLen = 0;

        ConstString APPLICATION_TITLE = CONFIG_VALUE( "Window", "ApplicationTitleTextId", STRINGIZE_STRING_LOCAL( "APPLICATION_TITLE" ) );

        TextEntryInterfacePtr entry;
        if( TEXT_SERVICE()
            ->hasTextEntry( APPLICATION_TITLE, &entry ) == false )
        {
            LOGGER_MESSAGE( "application not setup title '%s'"
                , APPLICATION_TITLE.c_str()
            );
        }
        else
        {
            projectTitle = entry->getValue( &projectTitleLen );
            
            LOGGER_INFO( "application", "project title '%.*s'"
                , (int32_t)projectTitleLen
                , projectTitle
            );
        }

        PLATFORM_SERVICE()
            ->setProjectTitle( projectTitle );

        if( PLATFORM_SERVICE()
            ->alreadyRunningMonitor() == false )
        {
            return true;
        }

#if defined(MENGINE_PLATFORM_IOS)
        if( PLATFORM_SERVICE()
            ->createWindow( Resolution( 0, 0 ), true ) == false )
        {
            LOGGER_FATAL( "invalid create window" );

            return false;
        }
#elif defined(MENGINE_PLATFORM_ANDROID)
        if( PLATFORM_SERVICE()
            ->createWindow( Resolution( 0, 0 ), true ) == false )
        {
            LOGGER_FATAL( "invalid create window" );

            return false;
        }
#else
        bool fullscreen = APPLICATION_SERVICE()
            ->getFullscreenMode();

        Resolution windowResolution;
        if( APPLICATION_SERVICE()
            ->calcWindowResolution( fullscreen, &windowResolution ) == false )
        {
            LOGGER_FATAL( "invalid calculate window resolution for fullscreen [%s]"
                , fullscreen == true ? "YES" : "NO"
            );

            return false;
        }

        if( PLATFORM_SERVICE()
            ->createWindow( windowResolution, fullscreen ) == false )
        {
            LOGGER_FATAL( "invalid create window [%u:%u] fullscreen [%s]"
                , windowResolution.getWidth()
                , windowResolution.getHeight()
                , fullscreen == true ? "YES" : "NO"
            );

            return false;
        }
#endif

        if( APPLICATION_SERVICE()
            ->createRenderWindow() == false )
        {
            LOGGER_FATAL( "invalid create render window" );

            return false;
        }

        APPLICATION_SERVICE()
            ->turnSound( true );

        GAME_SERVICE()
            ->run();

        PLATFORM_SERVICE()
            ->runPlatform();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLApplication::loop()
    {
        PLATFORM_SERVICE()
            ->loopPlatform();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLApplication::finalize()
    {
        if( SERVICE_PROVIDER_EXIST() == false )
        {
            return;
        }
        
        if( SERVICE_IS_INITIALIZE( Mengine::PlatformServiceInterface ) == true )
        {
            PLATFORM_SERVICE()
                ->stopPlatform();
        }

        if( SERVICE_IS_INITIALIZE( Mengine::BootstrapperInterface ) == true )
        {
            BOOTSTRAPPER_SERVICE()
                ->stop();
        }

#if defined(MENGINE_PLUGIN_MENGINE_DLL)
#   error "MENGINE_PLUGIN_MENGINE_DLL for SDL not implemented"
#elif defined(MENGINE_PLUGIN_MENGINE_STATIC)
        finalizeMengine();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
