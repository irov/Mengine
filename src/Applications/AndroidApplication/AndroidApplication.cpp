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

#include "Mengine/MenginePlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/StringArguments.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/Error.h"

#include "Config/Algorithm.h"
#include "Config/StdString.h"

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
#   include <dlfcn.h>
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidApplication::AndroidApplication()
#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
        : m_handleLibrary( nullptr )
#endif
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
    bool AndroidApplication::bootstrap( const Mengine::Char * _nativeLibraryDir, int32_t _argc, Char ** const _argv )
    {
        MENGINE_UNUSED( _nativeLibraryDir );

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
        Char mengineLibraryPath[MENGINE_MAX_PATH] = {'\0'};
        Mengine::StdString::strcpy( mengineLibraryPath, _nativeLibraryDir );
        Mengine::StdString::strcat( mengineLibraryPath, "/libMengine.so" );

        void * handleLibrary = ::dlopen(mengineLibraryPath, RTLD_GLOBAL);

        if( handleLibrary == nullptr )
        {
            __android_log_print( ANDROID_LOG_ERROR, "Mengine", "dlopen 'libMengine.so' failed" );

            return false;
        }

        m_handleLibrary = handleLibrary;
#endif

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
        FAPI_MengineCreate API_MengineCreate = (FAPI_MengineCreate)::dlsym( m_handleLibrary, "API_MengineCreate" );

        if( API_MengineCreate == nullptr )
        {
            return false;
        }
#endif

        ServiceProviderInterface * serviceProvider = API_MengineCreate();

        if( serviceProvider == nullptr )
        {
            return false;
        }

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

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
        FAPI_MengineBootstrap API_MengineBootstrap = (FAPI_MengineBootstrap)::dlsym( m_handleLibrary, "API_MengineBootstrap" );

        if( API_MengineBootstrap == nullptr )
        {
            return false;
        }
#endif

        if( API_MengineBootstrap() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidApplication::initialize()
    {
#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
        FAPI_MengineRun API_MengineRun = (FAPI_MengineRun)::dlsym( m_handleLibrary, "API_MengineRun" );

        if( API_MengineRun == nullptr )
        {
            return false;
        }
#endif

        if( API_MengineRun() == false )
        {
            return false;
        }

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
        }

        PLATFORM_SERVICE()
            ->setProjectTitle( projectTitle );

        PLATFORM_SERVICE()
            ->updatePlatform();

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

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
        FAPI_MengineFinalize API_MengineFinalize = (FAPI_MengineFinalize)::dlsym( m_handleLibrary, "API_MengineFinalize" );

        if( API_MengineFinalize != nullptr )
        {
            API_MengineFinalize();
        }
#else
        API_MengineFinalize();
#endif

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
        ::dlclose(m_handleLibrary);
        m_handleLibrary = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
