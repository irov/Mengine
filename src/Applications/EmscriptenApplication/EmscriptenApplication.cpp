#include "EmscriptenApplication.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/BootstrapperInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include "Environment/Emscripten/EmscriptenIncluder.h"

#include "Configuration/Configurations.h"

#include "Runtime/MengineRuntime.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/StringArguments.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"
#include "Kernel/StdioLogger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    const float MAX_FRAME_TIME = 100.f;
    //////////////////////////////////////////////////////////////////////////
    EmscriptenApplication::EmscriptenApplication()
        : m_previousTime( 0.0 )
        , m_finalized( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    EmscriptenApplication::~EmscriptenApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenApplication::initializeOptionsService_( int32_t _argc, Char ** const _argv )
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
    bool EmscriptenApplication::initializeLoggerService_()
    {
        if( LOGGER_SERVICE()
            ->isSilent() == true )
        {
            return true;
        }

        LoggerInterfacePtr loggerStdio = Helper::makeFactorableUnique<StdioLogger>( MENGINE_DOCUMENT_FUNCTION );

        loggerStdio->setWriteHistory( true );

        if( LOGGER_SERVICE()
            ->registerLogger( loggerStdio ) == false )
        {
            return true;
        }

        m_loggerStdio = loggerStdio;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenApplication::finalizeLoggerService_()
    {
        if( m_loggerStdio == nullptr )
        {
            return;
        }

        LOGGER_SERVICE()
            ->unregisterLogger( m_loggerStdio );

        m_loggerStdio = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenApplication::bootstrap( int32_t _argc, Char ** const _argv, const Configuration & _configuration )
    {
        ServiceProviderInterface * serviceProvider = ::API_MengineCreate( &_configuration );

        if( serviceProvider == nullptr )
        {
            return false;
        }

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_PROVIDER_GET()
            ->waitService( "EmscriptenApplication", SERVICE_ID( OptionsServiceInterface ), [this, _argc, _argv]()
        {
            if( this->initializeOptionsService_( _argc, _argv ) == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_WAIT( EmscriptenApplication, LoggerServiceInterface, [this]()
        {
            if( this->initializeLoggerService_() == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_LEAVE( EmscriptenApplication, LoggerServiceInterface, [this]()
        {
            this->finalizeLoggerService_();
        } );

        if( ::API_MengineBootstrap() == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool EmscriptenApplication::initialize()
    {
        if( BOOTSTRAPPER_SERVICE()
            ->initializeResources() == false )
        {
            return false;
        }

        if( ::API_MengineRun() == false )
        {
            return false;
        }

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

            const Char * title = projectTitle.c_str();
            PLATFORM_SERVICE()
                ->setProjectTitle( title );
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
    void EmscriptenApplication::loop()
    {
        m_previousTime = emscripten_get_now();

        emscripten_set_main_loop_arg( []( void * _user )
        {
            EmscriptenApplication * application = static_cast<EmscriptenApplication *>(_user);
            application->frame();
        }, this, 0, true );
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenApplication::frame()
    {
        if( PLATFORM_SERVICE()
            ->updatePlatform() == false )
        {
            this->finalize();

            return;
        }

        double now = emscripten_get_now();
        float elapsed = (float)(now - m_previousTime);
        m_previousTime = now;

        float frameTime = MENGINE_MIN_MAX( elapsed, 0.f, MAX_FRAME_TIME );

        PLATFORM_SERVICE()
            ->tickPlatform( frameTime );

        RenderPlatformInterface * renderPlatform = PLATFORM_SERVICE()
            ->getRenderPlatform();
        renderPlatform->renderPlatform();
    }
    //////////////////////////////////////////////////////////////////////////
    void EmscriptenApplication::finalize()
    {
        if( m_finalized == true )
        {
            return;
        }

        m_finalized = true;
        emscripten_cancel_main_loop();

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

            BOOTSTRAPPER_SERVICE()
                ->finalize();
        }

        ::API_MengineFinalize();
    }
    //////////////////////////////////////////////////////////////////////////
}
