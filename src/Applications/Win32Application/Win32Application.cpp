#include "Win32Application.h"

#include "Interface/PlatformServiceInterface.h"
#include "Interface/BootstrapperInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/ApplicationInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/OptionsServiceInterface.h"
#include "Interface/LoggerServiceInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/FileServiceInterface.h"

#include "Environment/Windows/Win32Helper.h"
#include "Environment/Windows/Win32MessageBoxLogger.h"
#include "Environment/Windows/Win32PlatformServiceExtensionInterface.h"

#if defined(MENGINE_WINDOWS_DEBUG)
#   include "Environment/Windows/Win32OutputDebugLogger.h"
#endif

#include "Mengine/MenginePlugin.h"

#include "Win32ExtraFileLogger.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/StringArguments.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"

#include "Config/Algorithm.h"
#include "Config/StdIO.h"
#include "Config/StdIntTypes.h"

#include <memory>
#include <cerrno>
#include <ctime>
#include <functional>

#include "resource.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_DLL_NAME
#define MENGINE_DLL_NAME L"Mengine.dll"
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32Application::Win32Application()
#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
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
            LOGGER_ASSERTION( "invalid CommandLineToArgvW [%ls] get error %ls"
                , lpCmdLine
                , Helper::Win32GetLastErrorMessageW()
            );

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
                LOGGER_ASSERTION( "unicode '%ls' WideCharToMultiByte [CP_UTF8] get error %ls"
                    , arg
                    , Helper::Win32GetLastErrorMessageW()
                );

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

        if( HAS_OPTION( "extralog" ) == true )
        {
            Win32ExtraFileLoggerPtr extraFileLogger = Helper::makeFactorableUnique<Win32ExtraFileLogger>( MENGINE_DOCUMENT_FUNCTION );

            extraFileLogger->setVerboseLevel( LM_VERBOSE );

            if( LOGGER_SERVICE()
                ->registerLogger( extraFileLogger ) == true )
            {
                m_loggerExtraFile = extraFileLogger;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Application::finalizeLoggerService_()
    {
        if( m_loggerMessageBox != nullptr )
        {
            if( SERVICE_IS_INITIALIZE( LoggerServiceInterface ) == true )
            {
                LOGGER_SERVICE()
                    ->unregisterLogger( m_loggerMessageBox );
            }

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

        if( m_loggerExtraFile != nullptr )
        {
            if( SERVICE_IS_INITIALIZE( LoggerServiceInterface ) == true )
            {
                LOGGER_SERVICE()
                    ->unregisterLogger( m_loggerExtraFile );
            }

            m_loggerExtraFile = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::initialize()
    {
#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
        HINSTANCE hInstance = ::LoadLibrary( MENGINE_DLL_NAME );

        if( hInstance == NULL )
        {
            DWORD error_code = ::GetLastError();

            MENGINE_UNUSED( error_code );

            WChar message[1024] = {L'\0'};
            MENGINE_SWPRINTF( message, 1024, L"Invalid load %ls [%lu]"
                , MENGINE_DLL_NAME
                , error_code
            );

            ::MessageBox( NULL, message, L"Mengine Error", MB_OK );

            return false;
        }

        m_hInstance = hInstance;
#endif

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
        FAPI_MengineCreate API_MengineCreate = (FAPI_MengineCreate)::GetProcAddress( m_hInstance, "API_MengineCreate" );

        if( API_MengineCreate == nullptr )
        {
            DWORD error_code = ::GetLastError();

            MENGINE_UNUSED( error_code );

            WChar message[1024] = {L'\0'};
            MENGINE_SWPRINTF( message, 1024, L"Invalid get function '%hs' from dll '%ls' [%lu]"
                , "initializeMengine"
                , MENGINE_DLL_NAME
                , error_code
            );

            ::MessageBox( NULL, message, L"Mengine Error", MB_OK );

            return false;
        }
#endif

        ServiceProviderInterface * serviceProvider = API_MengineCreate();

        if( serviceProvider == nullptr )
        {
            return false;
        }

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

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
        FAPI_MengineBootstrap API_MengineBootstrap = (FAPI_MengineBootstrap)::GetProcAddress( m_hInstance, "API_MengineBootstrap" );

        if( API_MengineBootstrap == nullptr )
        {
            return false;
        }
#endif

        if( API_MengineBootstrap() == false )
        {
            return false;
        }

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
        FAPI_MengineRun API_MengineRun = (FAPI_MengineRun)::GetProcAddress( m_hInstance, "API_MengineRun" );

        if( API_MengineRun == nullptr )
        {
            return false;
        }
#endif

        if( API_MengineRun() == false )
        {
            return false;
        }

        LOGGER_INFO( "system", "creating render window..." );

        ConstString Window_ApplicationTitleTextId = CONFIG_VALUE( "Window", "ApplicationTitleTextId", STRINGIZE_STRING_LOCAL( "APPLICATION_TITLE" ) );

        const Char * projectTitle = nullptr;
        size_t projectTitleLen = 0;

        TextEntryInterfacePtr entry;
        if( TEXT_SERVICE()
            ->hasTextEntry( Window_ApplicationTitleTextId, &entry ) == false )
        {
            LOGGER_INFO( "system", "Application not setup title '%s'"
                , Window_ApplicationTitleTextId.c_str()
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

        Win32PlatformServiceExtensionInterface * win32PlatformExtension = PLATFORM_SERVICE()
            ->getUnknown();

        win32PlatformExtension->setHWNDIcon( MAKEINTRESOURCEW( IDI_MENGINE ) );

        bool fullscreen = APPLICATION_SERVICE()
            ->getFullscreenMode();

        Resolution windowResolution;
        if( APPLICATION_SERVICE()
            ->calcWindowResolution( fullscreen, &windowResolution ) == false )
        {
            LOGGER_FATAL( "invalid calculate window resolution for fullscreen [%s]"
                , fullscreen == true ? "YES" : "NO"
            );

            return false;
        }

        if( PLATFORM_SERVICE()
            ->createWindow( windowResolution, fullscreen ) == false )
        {
            LOGGER_FATAL( "invalid create window [%u:%u] fullscreen [%s]"
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

        GAME_SERVICE()
            ->run();

        if( PLATFORM_SERVICE()
            ->runPlatform() == false )
        {
            LOGGER_FATAL( "invalid run platform" );

            return false;
        }

        Timestamp runPlatformTimeMilliseconds = PLATFORM_SERVICE()
            ->getPlatfomTime();

        Timestamp runPlatformTimeSeconds = runPlatformTimeMilliseconds / 1000;

        LOGGER_INFO( "system", "run platform time: %" MENGINE_PRIu64 " seconds"
            , runPlatformTimeSeconds
        );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Application::loop()
    {
        PLATFORM_SERVICE()
            ->loopPlatform();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Application::finalize()
    {
        if( SERVICE_PROVIDER_EXIST() == true )
        {
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

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
        FAPI_MengineFinalize API_MengineFinalize = (FAPI_MengineFinalize)::GetProcAddress( m_hInstance, "API_MengineFinalize" );

        if( API_MengineFinalize != nullptr )
        {
            API_MengineFinalize();
        }
#else
        API_MengineFinalize();
#endif

#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
        ::FreeLibrary( m_hInstance );
        m_hInstance = NULL;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
