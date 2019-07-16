#include "Config/Config.h"

#include "SDLApplication.h"

#include "Engine/Application.h"

#include "Interface/LoggerInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
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

#include <cstdio>
#include <clocale>

#include "Kernel/FactorableUnique.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/StringArguments.h"
#include "Kernel/FileLogger.h"
#include "Kernel/Document.h"
#include "Kernel/Logger.h"

#include "SDLMessageBoxLogger.h"

#ifdef MENGINE_PLATFORM_ANDROID
#   include "AndroidLogger.h"
#else
#   include "SDLStdioLogger.h"
#endif

#include <ctime>
#include <algorithm>

#include <sstream>
#include <iomanip>

#include "SDL_filesystem.h"

#ifndef MENGINE_APPLICATION_INI_PATH
#define MENGINE_APPLICATION_INI_PATH "application.ini"
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( Bootstrapper );
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT( SDLFileGroup );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    static void s_stdex_thread_lock( ThreadMutexInterface * _mutex )
    {
        _mutex->lock();
    }
    //////////////////////////////////////////////////////////////////////////
    static void s_stdex_thread_unlock( ThreadMutexInterface * _mutex )
    {
        _mutex->unlock();
    }
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
    bool SDLApplication::initializeOptionsService_( const int argc, char ** argv )
    {
        ArgumentsInterfacePtr arguments = Helper::makeFactorableUnique<StringArguments>();

        for( int i = 1; i < argc; ++i )
        {
            const Char * arg = argv[i];

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
        PLUGIN_CREATE( SDLFileGroup );

        // mount root
        ConstString c_dir = Helper::stringizeString( "dir" );
        if( FILE_SERVICE()
            ->mountFileGroup( ConstString::none(), nullptr, FilePath( ConstString::none() ), c_dir, nullptr, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            LOGGER_ERROR( "failed to mount application directory"
            );

            return false;
        }

#ifndef MENGINE_MASTER_RELEASE
        const FileGroupInterfacePtr & defaultFileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        ConstString c_dev = Helper::stringizeString( "dev" );
        // mount root
        if( FILE_SERVICE()
            ->mountFileGroup( c_dev, defaultFileGroup, FilePath( ConstString::none() ), c_dir, nullptr, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            LOGGER_ERROR( "failed to mount dev directory"
            );

            return false;
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::loadApplicationConfig_()
    {
        FilePath applicationPath = STRINGIZE_FILEPATH_LOCAL( MENGINE_APPLICATION_INI_PATH );

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        if( fileGroup->existFile( applicationPath ) == false )
        {
            LOGGER_INFO( "not exist application config '%s'"
                , applicationPath.c_str()
            );

            return true;
        }

        ConfigInterfacePtr config = CONFIG_SERVICE()
            ->createConfig( fileGroup, applicationPath, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( config, false, "invalid open application settings '%s'"
            , applicationPath.c_str()
        );

        config->getValues( "Game", "Path", m_configPaths );
        config->getValues( "Config", "Path", m_configPaths );
        config->getValues( "Resource", "Path", m_resourceConfigPaths );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeConfigService_()
    {
        LOGGER_WARNING( "Inititalizing Config Service..." );

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        for( const FilePath & filePath : m_configPaths )
        {
            if( CONFIG_SERVICE()
                ->loadDefaultConfig( fileGroup, filePath, MENGINE_DOCUMENT_FUNCTION ) == false )
            {
                LOGGER_ERROR( "invalid load config %s"
                    , filePath.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeUserDirectory_()
    {
        Char userPath[MENGINE_MAX_PATH];
        size_t userPathLen = PLATFORM_SERVICE()
            ->getUserPath( userPath );

        FilePath cs_userPath = Helper::stringizeFilePathSize( userPath, userPathLen );

        // mount user directory
        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "user" ), nullptr, cs_userPath, STRINGIZE_STRING_LOCAL( "global" ), nullptr, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            LOGGER_ERROR( "failed to mount user directory '%s'"
                , cs_userPath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeLoggerFile_()
    {
        bool nologs = HAS_OPTION( "nologs" );

        if( nologs == true )
        {
            return true;
        }

        m_fileLog = new FactorableUnique<FileLogger>();

        LOGGER_SERVICE()
            ->registerLogger( m_fileLog );

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
        m_loggerStdio = new FactorableUnique<AndroidLogger>();
#else
        m_loggerStdio = new FactorableUnique<SDLStdioLogger>();
#endif

        m_loggerStdio->setVerboseFlag( LM_MESSAGE );

        LOGGER_SERVICE()
            ->registerLogger( m_loggerStdio );

        m_loggerMessageBox = new FactorableUnique<SDLMessageBoxLogger>();

        m_loggerMessageBox->setVerboseLevel( LM_CRITICAL );

        LOGGER_SERVICE()
            ->registerLogger( m_loggerMessageBox );        

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initialize( const int argc, char ** argv )
    {
        setlocale( LC_ALL, "C" );

        ServiceProviderInterface * serviceProvider;
        if( SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider ) == false )
        {
            return false;
        }

        SERVICE_PROVIDER_SETUP( serviceProvider );

        m_serviceProvider = serviceProvider;

        SERVICE_PROVIDER_GET()
            ->waitService( OptionsServiceInterface::getStaticServiceID(), [this, argc, argv]()
        {
            if( this->initializeOptionsService_( argc, argv ) == false )
            {
                return false;
            }

            return true;
        } );

        SERVICE_WAIT( LoggerServiceInterface, [this]()
        {
            if( this->initializeLoggerService_() == false )
            {
                return false;
            }

            return true;
        } );

        SERVICE_WAIT( FileServiceInterface, [this]()
        {
            if( this->initializeFileService_() == false )
            {
                return false;
            }

            return true;
        } );

        SERVICE_WAIT( ConfigServiceInterface, [this]()
        {
            if( this->loadApplicationConfig_() == false )
            {
                return false;
            }

            if( this->initializeConfigService_() == false )
            {
                return false;
            }

            if( this->initializeUserDirectory_() == false )
            {
                return false;
            }

            if( this->initializeLoggerFile_() == false )
            {
                return false;
            }

            return true;
        } );

        SERVICE_WAIT( ThreadServiceInterface, [this]()
        {
            m_mutexAllocatorPool = THREAD_SERVICE()
                ->createMutex( MENGINE_DOCUMENT_FUNCTION );

            stdex_allocator_initialize_threadsafe( m_mutexAllocatorPool.get()
                , (stdex_allocator_thread_lock_t)& s_stdex_thread_lock
                , (stdex_allocator_thread_unlock_t)& s_stdex_thread_unlock
            );

            return true;
        } );

        SERVICE_CREATE( Bootstrapper );

        if( BOOTSTRAPPER_SERVICE()
            ->run() == false )
        {
            LOGGER_CRITICAL( "invalid bootstrap"
            );

            return false;
        }
        
        LOGGER_INFO( "initialize Game..." );

        const FileGroupInterfacePtr & defaultFileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        if( APPLICATION_SERVICE()
            ->initializeGame( defaultFileGroup, m_resourceConfigPaths ) == false )
        {
            LOGGER_CRITICAL( "Application invalid initialize game"
            );

            return false;
        }

        if( FRAMEWORK_SERVICE()
            ->onFrameworkInitialize() == false )
        {
            return false;
        }

        if( GAME_SERVICE()
            ->loadPersonality() == false )
        {
            LOGGER_CRITICAL( "Game invalid load personality"
            );

            return false;
        }

        PLATFORM_SERVICE()
            ->setIcon( 0 );

        String projectTitle;

        TextEntryInterfacePtr entry;
        if( TEXT_SERVICE()
            ->hasTextEntry( STRINGIZE_STRING_LOCAL( "APPLICATION_TITLE" ), &entry ) == false )
        {
            LOGGER_WARNING( "Application not setup title 'APPLICATION_TITLE'"
            );
        }
        else
        {
            projectTitle = entry->getValue();
        }

        PLATFORM_SERVICE()
            ->setProjectTitle( projectTitle.c_str() );

        const Resolution & windowResolution = APPLICATION_SERVICE()
            ->getCurrentResolution();

        bool fullscreen = APPLICATION_SERVICE()
            ->getFullscreenMode();

        if( PLATFORM_SERVICE()
            ->createWindow( windowResolution, fullscreen ) == false )
        {
            return false;
        }

        LOGGER_INFO( "Creating Render Window..." );

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
        SERVICE_LEAVE( FileServiceInterface, [this]()
        {
            if( m_fileLog != nullptr )
            {
                LOGGER_SERVICE()
                    ->unregisterLogger( m_fileLog );

                m_fileLog = nullptr;
            }
        } );

        SERVICE_LEAVE( LoggerServiceInterface, [this]()
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
        } );

        SERVICE_LEAVE( ThreadServiceInterface, [this]()
        {
            stdex_allocator_finalize_threadsafe();

            m_mutexAllocatorPool = nullptr;
        } );

        BOOTSTRAPPER_SERVICE()
            ->stop();

        SERVICE_FINALIZE( Bootstrapper );

        SERVICE_PROVIDER_FINALIZE( m_serviceProvider );
    }
}
