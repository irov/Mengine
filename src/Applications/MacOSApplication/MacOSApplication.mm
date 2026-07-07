#import "MacOSApplication.h"

#import "Configuration/Configurations.h"

#import "Interface/PlatformServiceInterface.h"
#import "Interface/BootstrapperInterface.h"
#import "Interface/TextServiceInterface.h"
#import "Interface/ApplicationInterface.h"
#import "Interface/OptionsServiceInterface.h"
#import "Interface/LoggerServiceInterface.h"

#import "MacOSApplicationDelegate.h"
#import "MacOSMessageBoxLogger.h"

#import "Mengine/MenginePlugin.h"

#import "Kernel/ConfigHelper.h"
#import "Kernel/StringArguments.h"
#import "Kernel/FactorableUnique.h"
#import "Kernel/Logger.h"
#import "Kernel/StdioLogger.h"

#import "Config/StdString.h"

#import <AppKit/AppKit.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MacOSApplication::MacOSApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MacOSApplication::~MacOSApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSApplication::initializeOptionsService_( int32_t _argc, Char ** const _argv )
    {
        ArgumentsInterfacePtr arguments = Helper::makeFactorableUnique<StringArguments>( MENGINE_DOCUMENT_FUNCTION );

        for( int32_t i = 1; i < _argc; ++i )
        {
            const Char * arg = _argv[i];

            if( StdString::strncmp( arg, "-NS", 3 ) == 0 )
            {
                continue;
            }

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
    bool MacOSApplication::initializeLoggerService_()
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

        LoggerInterfacePtr loggerMessageBox = Helper::makeFactorableUnique<MacOSMessageBoxLogger>( MENGINE_DOCUMENT_FUNCTION );

        loggerMessageBox->setVerboseLevel( LM_FATAL );

        if( LOGGER_SERVICE()
            ->registerLogger( loggerMessageBox ) == true )
        {
            m_loggerMessageBox = loggerMessageBox;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSApplication::finalizeLoggerService_()
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
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSApplication::bootstrap( int32_t _argc, Char ** const _argv )
    {
#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
#   error "MENGINE_PLUGIN_MENGINE_SHARED for MacOS not implemented"
#elif defined(MENGINE_PLUGIN_MENGINE_STATIC)
        ServiceProviderInterface * serviceProvider = ::API_MengineCreate( nullptr );
#else
        ServiceProviderInterface * serviceProvider = nullptr;
#endif

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_PROVIDER_GET()->waitService( "MacOSApplication", SERVICE_ID( OptionsServiceInterface ), [this, _argc, _argv]()
        {
            if( this->initializeOptionsService_( _argc, _argv ) == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_WAIT( MacOSApplication, LoggerServiceInterface, [this]()
        {
            if( this->initializeLoggerService_() == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_LEAVE( MacOSApplication, LoggerServiceInterface, [this]()
        {
            this->finalizeLoggerService_();
        } );

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
#   error "MENGINE_PLUGIN_MENGINE_SHARED for MacOS not implemented"
#elif defined(MENGINE_PLUGIN_MENGINE_STATIC)
        if( ::API_MengineBootstrap() == false )
        {
            return false;
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MacOSApplication::initialize()
    {
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];

        if( [NSApp delegate] == nil )
        {
            MacOSApplicationDelegate * applicationDelegate = [[MacOSApplicationDelegate alloc] init];
            [NSApp setDelegate:applicationDelegate];
        }

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
#   error "MENGINE_PLUGIN_MENGINE_SHARED for MacOS not implemented"
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

        [NSApp activateIgnoringOtherApps:YES];

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSApplication::loop()
    {
        PLATFORM_SERVICE()
            ->loopPlatform();
    }
    //////////////////////////////////////////////////////////////////////////
    void MacOSApplication::finalize()
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
#   error "MENGINE_PLUGIN_MENGINE_SHARED for MacOS not implemented"
#elif defined(MENGINE_PLUGIN_MENGINE_STATIC)
        ::API_MengineFinalize();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
