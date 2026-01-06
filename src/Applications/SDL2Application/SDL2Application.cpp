#include "SDL2Application.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/BootstrapperInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/FileServiceInterface.h"

#if defined(MENGINE_PLATFORM_IOS)
#   include "iOSLogger.h"
#else
#   include "Kernel/StdioLogger.h"
#endif

#if defined(MENGINE_WINDOWS_DEBUG)
#   include "Environment/Windows/Win32OutputDebugLogger.h"
#endif

#include "Mengine/MenginePlugin.h"

#include "Environment/SDL2/SDL2MessageBoxLogger.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/StringArguments.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/Error.h"

#include "Config/StdAlgorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL2Application::SDL2Application()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL2Application::~SDL2Application()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2Application::initializeOptionsService_( int32_t _argc, Char ** const _argv )
    {
        ArgumentsInterfacePtr arguments = Helper::makeFactorableUnique<StringArguments>( MENGINE_DOCUMENT_FUNCTION );

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
    bool SDL2Application::initializeLoggerService_()
    {
        if( LOGGER_SERVICE()
            ->isSilent() == true )
        {
            return true;
        }

#if defined(MENGINE_PLATFORM_IOS)
        LoggerInterfacePtr loggerStdio = Helper::makeFactorableUnique<iOSLogger>( MENGINE_DOCUMENT_FUNCTION );
#else
        LoggerInterfacePtr loggerStdio = Helper::makeFactorableUnique<StdioLogger>( MENGINE_DOCUMENT_FUNCTION );
#endif

        loggerStdio->setWriteHistory( true );

        if( LOGGER_SERVICE()
            ->registerLogger( loggerStdio ) == true )
        {
            m_loggerStdio = loggerStdio;
        }

        LoggerInterfacePtr loggerMessageBox = Helper::makeFactorableUnique<SDL2MessageBoxLogger>( MENGINE_DOCUMENT_FUNCTION );

        loggerMessageBox->setVerboseLevel( LM_FATAL );

        if( LOGGER_SERVICE()
            ->registerLogger( loggerMessageBox ) == true )
        {
            m_loggerMessageBox = loggerMessageBox;
        }

#if defined(MENGINE_WINDOWS_DEBUG)
        Win32OutputDebugLoggerPtr loggerOutputDebug = Helper::makeFactorableUnique<Win32OutputDebugLogger>( MENGINE_DOCUMENT_FUNCTION );

        loggerOutputDebug->setVerboseLevel( LM_MESSAGE );

        if( LOGGER_SERVICE()
            ->registerLogger( loggerOutputDebug ) == true )
        {
            m_loggerOutputDebug = loggerOutputDebug;
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2Application::finalizeLoggerService_()
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
    bool SDL2Application::bootstrap( int32_t _argc, Char ** const _argv )
    {
#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
#   error "MENGINE_PLUGIN_MENGINE_SHARED for SDL2 not implemented"
#elif defined(MENGINE_PLUGIN_MENGINE_STATIC)
        ServiceProviderInterface * serviceProvider = API_MengineCreate();
#else
        ServiceProviderInterface * serviceProvider = nullptr;
#endif

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_PROVIDER_GET()->waitService( "SDL2Application", SERVICE_ID( OptionsServiceInterface ), [this, _argc, _argv]()
        {
            if( this->initializeOptionsService_( _argc, _argv ) == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_WAIT( SDL2Application, LoggerServiceInterface, [this]()
        {
            if( this->initializeLoggerService_() == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_LEAVE( SDL2Application, LoggerServiceInterface, [this]()
        {
            this->finalizeLoggerService_();
        } );

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
#error "MENGINE_PLUGIN_MENGINE_SHARED for SDL2 not implemented"
#elif defined(MENGINE_PLUGIN_MENGINE_STATIC)
        if( ::API_MengineBootstrap() == false )
        {
            return false;
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2Application::initialize()
    {
#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
#error "MENGINE_PLUGIN_MENGINE_SHARED for SDL2 not implemented"
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

            const Char * projectTitleStr = projectTitle.c_str();

            PLATFORM_SERVICE()
                ->setProjectTitle( projectTitleStr );
        }

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
#else
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
    void SDL2Application::loop()
    {
        PLATFORM_SERVICE()
            ->loopPlatform();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2Application::finalize()
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
#   error "MENGINE_PLUGIN_MENGINE_SHARED for SDL2 not implemented"
#elif defined(MENGINE_PLUGIN_MENGINE_STATIC)
        API_MengineFinalize();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
