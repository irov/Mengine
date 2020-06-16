#include "Config/Config.h"

#include "SDLApplication.h"

#include "Engine/Application.h"

#include "Interface/LoggerInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/ConverterServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/PackageServiceInterface.h"
#include "Interface/UserdataServiceInterface.h"
#include "Interface/GraveyardServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/SoundServiceInterface.h"
#include "Interface/RenderServiceInterface.h"
#include "Interface/UpdateServiceInterface.h"
#include "Interface/ArchiveServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/TimeSystemInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ModuleServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/AccountServiceInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/EnumeratorServiceInterface.h"
#include "Interface/ChronometerServiceInterface.h"
#include "Interface/FrameworkInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/EasingServiceInterface.h"
#include "Interface/PickerServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/BootstrapperInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include <clocale>

#include "Kernel/FactorableUnique.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/StringArguments.h"
#include "Kernel/FileLogger.h"
#include "Kernel/Document.h"
#include "Kernel/Logger.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/Stringstream.h"

#include "SDLMessageBoxLogger.h"

#if defined(MENGINE_PLATFORM_ANDROID)
#   include "AndroidLogger.h"
#else
#   include "SDLStdioLogger.h"
#endif

#include <ctime>
#include <algorithm>

#include <sstream>
#include <iomanip>

#include "SDL_filesystem.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AllocatorService );
SERVICE_EXTERN( DocumentService );
SERVICE_EXTERN( Bootstrapper );
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT( SDLFileGroup );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLApplication::SDLApplication()
        : m_serviceProvider( nullptr )
        , m_running( false )
        , m_active( false )
        , m_developmentMode( false )
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
    bool SDLApplication::initializeFileService_()
    {
        LOGGER_INFO( "Initialize SDL file group..." );
        PLUGIN_CREATE( SDLFileGroup, MENGINE_DOCUMENT_FUNCTION );

        // mount root
        ConstString c_dir = STRINGIZE_STRING_LOCAL( "dir" );
        if( FILE_SERVICE()
            ->mountFileGroup( ConstString::none(), nullptr, nullptr, FilePath::none(), c_dir, nullptr, false, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            LOGGER_ERROR( "failed to mount application directory"
            );

            return false;
        }

#ifndef MENGINE_MASTER_RELEASE
        const FileGroupInterfacePtr & defaultFileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        ConstString c_dev = STRINGIZE_STRING_LOCAL( "dev" );
        // mount root
        if( FILE_SERVICE()
            ->mountFileGroup( c_dev, defaultFileGroup, nullptr, FilePath::none(), c_dir, nullptr, false, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            LOGGER_ERROR( "failed to mount dev directory"
            );

            return false;
        }
#endif

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
        ::setlocale( LC_ALL, "C" );

        ServiceProviderInterface * serviceProvider;
        if( SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider ) == false )
        {
            return false;
        }

        SERVICE_PROVIDER_SETUP( serviceProvider );

        m_serviceProvider = serviceProvider;

        SERVICE_CREATE( AllocatorService, nullptr );
        SERVICE_CREATE( DocumentService, nullptr );

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


        UNKNOWN_SERVICE_WAIT( SDLApplication, FileServiceInterface, [this]()
        {
            if( this->initializeFileService_() == false )
            {
                return false;
            }

            return true;
        } );

        SERVICE_CREATE( Bootstrapper, MENGINE_DOCUMENT_FUNCTION );

        if( BOOTSTRAPPER_SERVICE()
            ->run() == false )
        {
            LOGGER_CRITICAL( "invalid bootstrap"
            );

            return false;
        }

#ifdef MENGINE_GIT_SHA1
        LOGGER_INFO( "GIT_SHA1: %s", MENGINE_GIT_SHA1 );
#endif

        LOGGER_MESSAGE( "Creating Render Window..." );

        PLATFORM_SERVICE()
            ->setIcon( 0 );

        const Char * projectTitle = nullptr;
        size_t projectTitleLen = 0;

        TextEntryInterfacePtr entry;
        if( TEXT_SERVICE()
            ->hasTextEntry( STRINGIZE_STRING_LOCAL( "APPLICATION_TITLE" ), &entry ) == false )
        {
            LOGGER_INFO( "Application not setup title 'APPLICATION_TITLE'"
            );
        }
        else
        {
            projectTitle = entry->getValue( &projectTitleLen );
        }

        PLATFORM_SERVICE()
            ->setProjectTitle( projectTitle );

        Resolution windowResolution;
        APPLICATION_SERVICE()
            ->calcWindowResolution( &windowResolution );

        bool fullscreen = APPLICATION_SERVICE()
            ->getFullscreenMode();

        if( PLATFORM_SERVICE()
            ->createWindow( windowResolution, fullscreen ) == false )
        {
            return false;
        }

        if( APPLICATION_SERVICE()
            ->createRenderWindow() == false )
        {
            return false;
        }

        APPLICATION_SERVICE()
            ->turnSound( true );

        GAME_SERVICE()
            ->run();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLApplication::loop()
    {
        PLATFORM_SERVICE()
            ->runPlatform();

        PLATFORM_SERVICE()
            ->updatePlatform();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLApplication::finalize()
    {
        BOOTSTRAPPER_SERVICE()
            ->stop();

        SERVICE_FINALIZE( Bootstrapper );
        SERVICE_DESTROY( Bootstrapper );

        SERVICE_FINALIZE( DocumentService );
        SERVICE_DESTROY( DocumentService );

        SERVICE_FINALIZE( AllocatorService );
        SERVICE_DESTROY( AllocatorService );

        SERVICE_PROVIDER_FINALIZE( m_serviceProvider );
    }
}
