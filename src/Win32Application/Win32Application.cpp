#include "Win32Application.h"

#include "Interface/PlatformInterface.h"
#include "Interface/BootstrapperInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/FileServiceInterface.h"

#include "Win32MessageBoxLogger.h"

#include "Kernel/StringArguments.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"

#include <clocale>
#include <memory>
#include <cerrno>
#include <ctime>
#include <algorithm>
#include <functional>
#include <iomanip>

#include "resource.h"

//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_MENGINE_STATIC
extern Mengine::ServiceProviderInterface * initializeMengine();
extern bool bootstrapMengine();
extern void finalizeMengine();
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLUGIN_MENGINE_DLL
    //////////////////////////////////////////////////////////////////////////
    typedef ServiceProviderInterface * (*FMengineInitialize)(void);
    typedef bool (*FMengineBootstrap)(void);
    typedef void (*FMengineFinalize)(void);
    //////////////////////////////////////////////////////////////////////////
#endif
    //////////////////////////////////////////////////////////////////////////
    Win32Application::Win32Application()
#ifdef MENGINE_PLUGIN_MENGINE_DLL
        : m_hInstance( NULL )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32Application::~Win32Application()
    {
    }    
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::initializeOptionsService_()
    {
        LPCWSTR lpCmdLine = ::GetCommandLineW();

        if( lpCmdLine == NULL )
        {
            return false;
        }

        int32_t pNumArgs;
        LPWSTR * szArglist = ::CommandLineToArgvW( lpCmdLine, &pNumArgs );

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
            int32_t utf_arg_size = ::WideCharToMultiByte( CP_UTF8, dwConversionFlags, arg, -1, utf_arg, 1024, NULL, NULL );

            if( utf_arg_size <= 0 )
            {
                return false;
            }

            arguments->addArgument( utf_arg );
        }

        ::LocalFree( szArglist );

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

        Win32MessageBoxLoggerPtr loggerMessageBox = Helper::makeFactorableUnique<Win32MessageBoxLogger>( MENGINE_DOCUMENT_FUNCTION );

        loggerMessageBox->setVerboseLevel( LM_CRITICAL );

        LOGGER_SERVICE()
            ->registerLogger( loggerMessageBox );

        m_loggerMessageBox = loggerMessageBox;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Application::finalizeLoggerService_()
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
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::initialize()
    {
#ifdef MENGINE_PLUGIN_MENGINE_DLL
        HINSTANCE hInstance = ::LoadLibrary( L"Mengine.dll" );

        if( hInstance == NULL )
        {
            return false;
        }

        FARPROC procInitializeMengine = ::GetProcAddress( hInstance, "initializeMengine" );

        if( procInitializeMengine == nullptr )
        {
            ::FreeLibrary( hInstance );

            return false;
        }

        FMengineInitialize dlmengineInitialize = (FMengineInitialize)procInitializeMengine;

        ServiceProviderInterface * serviceProvider = dlmengineInitialize();

        if( serviceProvider == nullptr )
        {
            ::FreeLibrary( hInstance );

            return false;
        }
#else
        ServiceProviderInterface * serviceProvider = initializeMengine();
#endif

        SERVICE_PROVIDER_SETUP( serviceProvider );

        UNKNOWN_SERVICE_WAIT( Win32Application, OptionsServiceInterface, [this]()
        {
            if( this->initializeOptionsService_() == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_WAIT( Win32Application, LoggerServiceInterface, [this]()
        {
            if( this->initializeLoggerService_() == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_LEAVE( Win32Application, LoggerServiceInterface, [this]()
        {
            this->finalizeLoggerService_();
        } );

#ifdef MENGINE_PLUGIN_MENGINE_DLL
        FARPROC procBootstrapMengine = ::GetProcAddress( hInstance, "bootstrapMengine" );

        if( procBootstrapMengine == nullptr )
        {
            ::FreeLibrary( hInstance );

            return false;
        }

        FMengineBootstrap dlmengineBootstrap = (FMengineBootstrap)procBootstrapMengine;

        if( dlmengineBootstrap() == false )
        {
            ::FreeLibrary( hInstance );

            return false;
        }

        m_hInstance = hInstance;
#else
        if( bootstrapMengine() == false )
        {
            return false;
        }
#endif

        LOGGER_MESSAGE( "Creating Render Window..." );

        const Char * projectTitle = nullptr;
        size_t projectTitleLen = 0;

        ConstString APPLICATION_TITLE = STRINGIZE_STRING_LOCAL( "APPLICATION_TITLE" );

        TextEntryInterfacePtr entry;
        if( TEXT_SERVICE()
            ->hasTextEntry( APPLICATION_TITLE, &entry ) == false )
        {
            LOGGER_INFO( "system", "Application not setup title '%s'"
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
            ->setIcon( IDI_MENGINE );

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
            LOGGER_CRITICAL( "Invalid create render window" );

            return false;
        }

        GAME_SERVICE()
            ->run();

        PLATFORM_SERVICE()
            ->runPlatform();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Application::loop()
    {
        PLATFORM_SERVICE()
            ->updatePlatform();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Application::finalize()
    {
        PLATFORM_SERVICE()
            ->stopPlatform();

        BOOTSTRAPPER_SERVICE()
            ->stop();

#ifdef MENGINE_PLUGIN_MENGINE_DLL
        FARPROC procFinalizeMengine = ::GetProcAddress( m_hInstance, "finalizeMengine" );

        if( procFinalizeMengine == nullptr )
        {
            return;
        }

        FMengineFinalize dlmengineFinalize = (FMengineFinalize)procFinalizeMengine;

        dlmengineFinalize();

        ::FreeLibrary( m_hInstance );
        m_hInstance = NULL;
#else
        finalizeMengine();
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
