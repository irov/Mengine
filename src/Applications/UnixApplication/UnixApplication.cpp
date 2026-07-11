#include "UnixApplication.h"

#include "Configuration/Configurations.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/BootstrapperInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include "Mengine/MenginePlugin.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/StringArguments.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/StdioLogger.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    UnixApplication::UnixApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UnixApplication::~UnixApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixApplication::initializeOptionsService_( int32_t _argc, Char ** const _argv )
    {
        ArgumentsInterfacePtr arguments = Helper::makeFactorableUnique<StringArguments>( MENGINE_DOCUMENT_FUNCTION );

        for( int32_t i = 1; i < _argc; ++i )
        {
            arguments->addArgument( _argv[i] );
        }

        if( OPTIONS_SERVICE()
            ->setArguments( arguments ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixApplication::initializeLoggerService_()
    {
        if( LOGGER_SERVICE()
            ->isSilent() == true )
        {
            return true;
        }

        LoggerInterfacePtr loggerStdio = Helper::makeFactorableUnique<StdioLogger>( MENGINE_DOCUMENT_FUNCTION );

        loggerStdio->setWriteHistory( true );

        if( LOGGER_SERVICE()
            ->registerLogger( loggerStdio ) == true )
        {
            m_loggerStdio = loggerStdio;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixApplication::finalizeLoggerService_()
    {
        if( m_loggerStdio != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_loggerStdio );

            m_loggerStdio = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixApplication::bootstrap( int32_t _argc, Char ** const _argv )
    {
#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
#   error "MENGINE_PLUGIN_MENGINE_SHARED for Unix not implemented"
#elif defined(MENGINE_PLUGIN_MENGINE_STATIC)
        ServiceProviderInterface * serviceProvider = ::API_MengineCreate( nullptr );
#else
        ServiceProviderInterface * serviceProvider = nullptr;
#endif

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_PROVIDER_GET()->waitService( "UnixApplication", SERVICE_ID( OptionsServiceInterface ), [this, _argc, _argv]()
        {
            if( this->initializeOptionsService_( _argc, _argv ) == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_WAIT( UnixApplication, LoggerServiceInterface, [this]()
        {
            if( this->initializeLoggerService_() == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_LEAVE( UnixApplication, LoggerServiceInterface, [this]()
        {
            this->finalizeLoggerService_();
        } );

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
#   error "MENGINE_PLUGIN_MENGINE_SHARED for Unix not implemented"
#elif defined(MENGINE_PLUGIN_MENGINE_STATIC)
        if( ::API_MengineBootstrap() == false )
        {
            return false;
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixApplication::initialize()
    {
#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
#   error "MENGINE_PLUGIN_MENGINE_SHARED for Unix not implemented"
#elif defined(MENGINE_PLUGIN_MENGINE_STATIC)
        if( ::API_MengineRun() == false )
        {
            return false;
        }
#endif

        LOGGER_INFO( "application", "creating render window..." );

        ConstString Window_ApplicationTitleTextId = CONFIG_VALUE_CONSTSTRING( "Window", "ApplicationTitleTextId", STRINGIZE_STRING_LOCAL( "APPLICATION_TITLE" ) );

        TextEntryInterfacePtr entry;
        if( TEXT_SERVICE()
            ->hasTextEntry( Window_ApplicationTitleTextId, &entry ) == false )
        {
            LOGGER_MESSAGE( "application not setup title '%s'"
                , Window_ApplicationTitleTextId.c_str()
            );
        }
        else
        {
            const String & projectTitle = entry->getValue();

            PLATFORM_SERVICE()
                ->setProjectTitle( projectTitle.c_str() );
        }

        if( PLATFORM_SERVICE()
            ->alreadyRunningMonitor() == false )
        {
            return true;
        }

        if( PLATFORM_SERVICE()
            ->updatePlatform() == false )
        {
            return false;
        }

        bool fullscreen = APPLICATION_SERVICE()
            ->getFullscreenMode();

        Resolution windowResolution;
        if( APPLICATION_SERVICE()
            ->calcWindowResolution( fullscreen, &windowResolution ) == false )
        {
            LOGGER_FATAL( "invalid calculate window resolution for fullscreen: %s"
                , fullscreen == true ? "YES" : "NO"
            );

            return false;
        }

        if( PLATFORM_SERVICE()
            ->createWindow( windowResolution, fullscreen ) == false )
        {
            LOGGER_FATAL( "invalid create window: %u:%u fullscreen: %s"
                , windowResolution.getWidth()
                , windowResolution.getHeight()
                , fullscreen == true ? "YES" : "NO"
            );

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

        if( PLATFORM_SERVICE()
            ->runPlatform() == false )
        {
            LOGGER_FATAL( "invalid run platform" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixApplication::loop()
    {
        PLATFORM_SERVICE()
            ->loopPlatform();
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixApplication::finalize()
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

        if( SERVICE_IS_INITIALIZE( Mengine::BootstrapperInterface ) == true )
        {
            BOOTSTRAPPER_SERVICE()
                ->finalize();
        }

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
#   error "MENGINE_PLUGIN_MENGINE_SHARED for Unix not implemented"
#elif defined(MENGINE_PLUGIN_MENGINE_STATIC)
        ::API_MengineFinalize();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
