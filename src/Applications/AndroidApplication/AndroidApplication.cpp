#include "AndroidApplication.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/BootstrapperInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/PreferencesSystemInterface.h"

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidLogger.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/StringArguments.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/Error.h"

#include "Config/Algorithm.h"

//////////////////////////////////////////////////////////////////////////
#if defined(MENGINE_PLUGIN_MENGINE_STATIC)
extern Mengine::ServiceProviderInterface * API_MengineCreate();
extern bool API_MengineBootstrap();
extern bool API_MengineRun();
extern void API_MengineFinalize();
#endif
//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
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
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidApplication::AndroidApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidApplication::~AndroidApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidApplication::initializeOptionsService_( int32_t _argc, Char ** const _argv )
    {
        ArgumentsInterfacePtr arguments = Helper::makeFactorableUnique<StringArguments>( MENGINE_DOCUMENT_FUNCTION );

        JNIEnv * jenv = Mengine_JNI_GetEnv();

        MENGINE_ASSERTION_MEMORY_PANIC( jenv, "invalid get jenv" );

        Detail::addAndroidBuildConfigOptions( jenv, "MENGINE_APP_OPTIONS", arguments );

#if !defined(MENGINE_BUILD_PUBLISH)
        Char MengineApplePersistentArguments[1024] = {'\0'};
        if( PREFERENCES_SYSTEM()
            ->getPreferenceString( "persistent_arguments", MengineApplePersistentArguments, 1024, nullptr ) == true )
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
    bool AndroidApplication::initializeLoggerService_()
    {
        if( LOGGER_SERVICE()
            ->isSilent() == true )
        {
            return true;
        }

        LoggerInterfacePtr loggerStdio = Helper::makeFactorableUnique<AndroidLogger>( MENGINE_DOCUMENT_FUNCTION );

        loggerStdio->setWriteHistory( true );

        if( LOGGER_SERVICE()
            ->registerLogger( loggerStdio ) == true )
        {
            m_loggerStdio = loggerStdio;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidApplication::finalizeLoggerService_()
    {
        if( m_loggerStdio != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_loggerStdio );

            m_loggerStdio = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidApplication::bootstrap( int32_t _argc, Char ** const _argv )
    {
#if defined(MENGINE_PLUGIN_MENGINE_DLL)
#   error "MENGINE_PLUGIN_MENGINE_DLL for SDL not implemented"
#elif defined(MENGINE_PLUGIN_MENGINE_STATIC)
        ServiceProviderInterface * serviceProvider = API_MengineCreate();
#else
        ServiceProviderInterface * serviceProvider = nullptr;
#endif

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_PROVIDER_GET()
            ->waitService( "AndroidApplication", SERVICE_ID( OptionsServiceInterface ), [this, _argc, _argv]()
        {
            if( this->initializeOptionsService_( _argc, _argv ) == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_WAIT( AndroidApplication, LoggerServiceInterface, [this]()
        {
            if( this->initializeLoggerService_() == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_LEAVE( AndroidApplication, LoggerServiceInterface, [this]()
        {
            this->finalizeLoggerService_();
        } );

#if defined(MENGINE_PLUGIN_MENGINE_DLL)
#error "MENGINE_PLUGIN_MENGINE_DLL for SDL not implemented"
#elif defined(MENGINE_PLUGIN_MENGINE_STATIC)
        if( ::API_MengineBootstrap() == false )
        {
            return false;
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidApplication::initialize()
    {
#if defined(MENGINE_PLUGIN_MENGINE_DLL)
#error "MENGINE_PLUGIN_MENGINE_DLL for SDL not implemented"
#elif defined(MENGINE_PLUGIN_MENGINE_STATIC)
        if( ::API_MengineRun() == false )
        {
            return false;
        }
#endif

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
            ->createWindow( Resolution( 0, 0 ), true ) == false )
        {
            LOGGER_FATAL( "invalid create window" );

            return false;
        }

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
    void AndroidApplication::loop()
    {
        PLATFORM_SERVICE()
            ->loopPlatform();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidApplication::finalize()
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
        API_MengineFinalize();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
