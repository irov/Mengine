#include "SDLApplication.h"

#include "Interface/PlatformInterface.h"
#include "Interface/BootstrapperInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/FileServiceInterface.h"

#include "SDLMessageBoxLogger.h"

#if defined(MENGINE_PLATFORM_ANDROID)
#   include "AndroidLogger.h"
#else
#   include "SDLStdioLogger.h"
#endif

#include "Kernel/StringArguments.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"

#include <ctime>
#include <algorithm>
#include <sstream>
#include <iomanip>

//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_MENGINE_STATIC
extern Mengine::ServiceProviderInterface * initializeMengine();
extern bool bootstrapMengine();
extern void finalizeMengine();
#endif
//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
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

        for( int32_t i = 1; i != _argc; ++i )
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
#else
        LoggerInterfacePtr loggerStdio = Helper::makeFactorableUnique<SDLStdioLogger>( MENGINE_DOCUMENT_FUNCTION );
#endif

        LOGGER_SERVICE()
            ->registerLogger( loggerStdio );

        m_loggerStdio = loggerStdio;

        LoggerInterfacePtr loggerMessageBox = Helper::makeFactorableUnique<SDLMessageBoxLogger>( MENGINE_DOCUMENT_FUNCTION );

        loggerMessageBox->setVerboseLevel( LM_CRITICAL );

        LOGGER_SERVICE()
            ->registerLogger( loggerMessageBox );

        m_loggerMessageBox = loggerMessageBox;

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
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initialize( int32_t _argc, Char ** const _argv )
    {
#ifdef MENGINE_PLUGIN_MENGINE_DLL
#error "MENGINE_PLUGIN_MENGINE_DLL for SDL not implemented"
#else
        ServiceProviderInterface * serviceProvider = initializeMengine();
#endif

        SERVICE_PROVIDER_SETUP( serviceProvider );

        SERVICE_PROVIDER_GET()->waitService( "SDLApplication", OptionsServiceInterface::getStaticServiceID(), [this, _argc, _argv]()
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

#ifdef MENGINE_PLUGIN_MENGINE_DLL
#error "MENGINE_PLUGIN_MENGINE_DLL for SDL not implemented"
#else
        if( bootstrapMengine() == false )
        {
            return false;
        }
#endif

        LOGGER_MESSAGE( "Creating Render Window..." );

        PLATFORM_SERVICE()
            ->setIcon( 0 );

        const Char * projectTitle = nullptr;
        size_t projectTitleLen = 0;

        ConstString APPLICATION_TITLE = STRINGIZE_STRING_LOCAL( "APPLICATION_TITLE" );

        TextEntryInterfacePtr entry;
        if( TEXT_SERVICE()
            ->hasTextEntry( APPLICATION_TITLE, &entry ) == false )
        {
            LOGGER_INFO( "application", "Application not setup title '%s'"
                , APPLICATION_TITLE.c_str()
            );
        }
        else
        {
            projectTitle = entry->getValue( &projectTitleLen );
        }

        PLATFORM_SERVICE()
            ->setProjectTitle( projectTitle );

        if( PLATFORM_SERVICE()
            ->alreadyRunningMonitor() == false )
        {
            return true;
        }

        bool fullscreen = APPLICATION_SERVICE()
            ->getFullscreenMode();

        Resolution windowResolution;
        if( APPLICATION_SERVICE()
            ->calcWindowResolution( fullscreen, &windowResolution ) == false )
        {
            return false;
        }

        if( PLATFORM_SERVICE()
            ->createWindow( windowResolution, fullscreen ) == false )
        {
            return false;
        }

        if( APPLICATION_SERVICE()
            ->createRenderWindow() == false )
        {
            LOGGER_CRITICAL( "invalid create render window" );

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
            ->updatePlatform();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLApplication::finalize()
    {
        PLATFORM_SERVICE()
            ->stopPlatform();

        BOOTSTRAPPER_SERVICE()
            ->stop();

#ifdef MENGINE_PLUGIN_MENGINE_DLL
#error "MENGINE_PLUGIN_MENGINE_DLL for SDL not implemented"
#else
        finalizeMengine();
#endif
    }
}
