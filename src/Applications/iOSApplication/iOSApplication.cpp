#include "iOSApplication.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/BootstrapperInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/FileServiceInterface.h"

#include "iOSLogger.h"

#include "Mengine/MenginePlugin.h"

#include "Environment/SDL3/SDL3MessageBoxLogger.h"

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
    iOSApplication::iOSApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    iOSApplication::~iOSApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool iOSApplication::initializeOptionsService_( int32_t _argc, Char ** const _argv )
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
    bool iOSApplication::initializeLoggerService_()
    {
        if( LOGGER_SERVICE()
            ->isSilent() == true )
        {
            return true;
        }

        LoggerInterfacePtr loggerStdio = Helper::makeFactorableUnique<iOSLogger>( MENGINE_DOCUMENT_FUNCTION );

        loggerStdio->setWriteHistory( true );

        if( LOGGER_SERVICE()
            ->registerLogger( loggerStdio ) == true )
        {
            m_loggerStdio = loggerStdio;
        }

        LoggerInterfacePtr loggerMessageBox = Helper::makeFactorableUnique<SDL3MessageBoxLogger>( MENGINE_DOCUMENT_FUNCTION );

        loggerMessageBox->setVerboseLevel( LM_FATAL );

        if( LOGGER_SERVICE()
            ->registerLogger( loggerMessageBox ) == true )
        {
            m_loggerMessageBox = loggerMessageBox;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::finalizeLoggerService_()
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
    bool iOSApplication::bootstrap( int32_t _argc, Char ** const _argv )
    {
        ServiceProviderInterface * serviceProvider = ::API_MengineCreate();

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_PROVIDER_GET()
            ->waitService( "iOSApplication", SERVICE_ID( OptionsServiceInterface ), [this, _argc, _argv]()
        {
            if( this->initializeOptionsService_( _argc, _argv ) == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_WAIT( iOSApplication, LoggerServiceInterface, [this]()
        {
            if( this->initializeLoggerService_() == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_LEAVE( iOSApplication, LoggerServiceInterface, [this]()
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
    bool iOSApplication::run()
    {
        if( ::API_MengineRun() == false )
        {
            return false;
        }

        LOGGER_INFO( "application", "creating render window..." );

        ConstString APPLICATION_TITLE = CONFIG_VALUE_CONSTSTRING( "Window", "ApplicationTitleTextId", STRINGIZE_STRING_LOCAL( "APPLICATION_TITLE" ) );

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
            const String & projectTitle = entry->getValue();
            
            const Char * projectTitle_str = projectTitle.c_str();
            
            PLATFORM_SERVICE()
                ->setProjectTitle( projectTitle_str );
        }

        if( PLATFORM_SERVICE()
            ->alreadyRunningMonitor() == false )
        {
            return true;
        }

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
    void iOSApplication::loop()
    {
        PLATFORM_SERVICE()
            ->loopPlatform();
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::stop()
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
    }
    //////////////////////////////////////////////////////////////////////////
    void iOSApplication::finalize()
    {
        if( SERVICE_PROVIDER_EXIST() == false )
        {
            return;
        }
        
        ::API_MengineFinalize();
    }
    //////////////////////////////////////////////////////////////////////////
}
