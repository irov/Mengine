#include "Win32Application.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/PackageServiceInterface.h"
#include "Interface/UserdataServiceInterface.h"
#include "Interface/GraveyardServiceInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/UpdateServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/DataServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/ConverterServiceInterface.h"
#include "Interface/AccountServiceInterface.h"
#include "Interface/EnumeratorServiceInterface.h"
#include "Interface/ChronometerServiceInterface.h"
#include "Interface/ModuleServiceInterface.h"
#include "Interface/FrameworkInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/EasingServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"
#include "Interface/BootstrapperInterface.h"
#include "Interface/LoggerServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/StringArguments.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/FileLogger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/Stringstream.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "MessageBoxLogger.h"

#include <clocale>
#include <memory>
#include <errno.h>
#include <ctime>
#include <algorithm>
#include <functional>
#include <iomanip>

#include "resource.h"

#ifndef MENGINE_SETLOCALE
#define MENGINE_SETLOCALE "C"
#endif

SERVICE_PROVIDER_EXTERN( ServiceProvider );

SERVICE_EXTERN( AllocatorService );
SERVICE_EXTERN( DocumentService );
SERVICE_EXTERN( Bootstrapper );

PLUGIN_EXPORT( Win32FileGroup );

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32Application::Win32Application()
        : m_serviceProvider( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32Application::~Win32Application()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::initializeFileService_()
    {
        LOGGER_INFO( "Inititalizing File Service..." );

        {
            LOGGER_INFO( "Initialize Win32 file group..." );
            PLUGIN_CREATE( Win32FileGroup, MENGINE_DOCUMENT_FUNCTION );
        }

        Char currentPath[MENGINE_MAX_PATH];
        size_t currentPathLen = PLATFORM_SERVICE()
            ->getCurrentPath( currentPath );

        if( currentPathLen == 0 )
        {
            LOGGER_ERROR( "failed to get current directory"
            );

            return false;
        }

        LOGGER_MESSAGE( "Current Path '%s'", currentPath );

        if( FILE_SERVICE()
            ->mountFileGroup( ConstString::none(), nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "dir" ), nullptr, false, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            LOGGER_ERROR( "failed to mount application directory '%s'"
                , currentPath
            );

            return false;
        }

#ifndef MENGINE_MASTER_RELEASE
        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ), nullptr, nullptr, FilePath::none(), STRINGIZE_STRING_LOCAL( "global" ), nullptr, false, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            LOGGER_ERROR( "failed to mount dev directory '%s'"
                , currentPath
            );

            return false;
        }
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::initializeOptionsService_()
    {
        LPCWSTR lpCmdLine = GetCommandLineW();

        if( lpCmdLine == NULL )
        {
            return false;
        }

        int32_t pNumArgs;
        LPWSTR * szArglist = CommandLineToArgvW( lpCmdLine, &pNumArgs );

        if( szArglist == NULL )
        {
            return false;
        }

#   if (WINVER >= 0x0600)
        DWORD dwConversionFlags = WC_ERR_INVALID_CHARS;
#   else
        DWORD dwConversionFlags = 0;
#   endif

        ArgumentsInterfacePtr arguments = Helper::makeFactorableUnique<StringArguments>( MENGINE_DOCUMENT_FUNCTION );

        for( int32_t i = 1; i != pNumArgs; ++i )
        {
            PWSTR arg = szArglist[i];

            CHAR utf_arg[1024];

            int32_t utf_arg_size = ::WideCharToMultiByte(
                CP_UTF8
                , dwConversionFlags
                , arg
                , -1
                , utf_arg
                , 1024
                , NULL
                , NULL
            );

            if( utf_arg_size <= 0 )
            {
                return false;
            }

            arguments->addArgument( utf_arg );
        }

        LocalFree( szArglist );

        if( OPTIONS_SERVICE()
            ->setArguments( arguments ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::initializeLoggerService_()
    {
        if( LOGGER_SERVICE()
            ->isSilent() == true )
        {
            return true;
        }

        MessageBoxLoggerPtr loggerMessageBox = Helper::makeFactorableUnique<MessageBoxLogger>( MENGINE_DOCUMENT_FUNCTION );

        loggerMessageBox->setVerboseLevel( LM_CRITICAL );

        LOGGER_SERVICE()
            ->registerLogger( loggerMessageBox );

        m_loggerMessageBox = loggerMessageBox;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::initialize()
    {
        ::setlocale( LC_ALL, MENGINE_SETLOCALE );

        ServiceProviderInterface * serviceProvider;
        if( SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider ) == false )
        {
            return false;
        }

        SERVICE_PROVIDER_SETUP( serviceProvider );

        m_serviceProvider = serviceProvider;

        SERVICE_CREATE( AllocatorService, nullptr );
        SERVICE_CREATE( DocumentService, nullptr );

        SERVICE_WAIT( OptionsServiceInterface, [this]()
        {
            if( this->initializeOptionsService_() == false )
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
            ->calcWindowResolution( windowResolution );

        PLATFORM_SERVICE()
            ->setIcon( IDI_MENGINE );

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
            LOGGER_CRITICAL( "not create render window"
            );

            return false;
        }

        bool vsync = APPLICATION_SERVICE()
            ->getVSync();

        bool maxfps = HAS_OPTION( "maxfps" );

        if( maxfps == true && vsync == true )
        {
            APPLICATION_SERVICE()
                ->setVSync( false );
        }

        GAME_SERVICE()
            ->run();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Application::loop()
    {
        PLATFORM_SERVICE()
            ->runPlatform();

        PLATFORM_SERVICE()
            ->updatePlatform();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Application::finalize()
    {
        SERVICE_LEAVE( LoggerServiceInterface, [this]()
        {
            if( m_loggerMessageBox != nullptr )
            {
                if( SERVICE_EXIST( LoggerServiceInterface ) == true )
                {
                    LOGGER_SERVICE()
                        ->unregisterLogger( m_loggerMessageBox );
                }

                m_loggerMessageBox = nullptr;
            }
        } );

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
