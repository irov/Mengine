#include "GDKApplication.h"

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
#include "Environment/Windows/Win32ConsoleLogger.h"
#include "Environment/Windows/Win32PlatformServiceExtensionInterface.h"

#if defined(MENGINE_WINDOWS_DEBUG)
#   include "Environment/Windows/Win32OutputDebugLogger.h"
#endif

#include "Mengine/MenginePlugin.h"

#include "Environment/Windows/Win32ExtraFileLogger.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/StringArguments.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdIO.h"
#include "Config/StdIntTypes.h"

#include <memory>
#include <cerrno>
#include <ctime>
#include <functional>

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_DLL_NAME
#define MENGINE_DLL_NAME L"Mengine.dll"
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GDKApplication::GDKApplication()
#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
        : m_hInstance( NULL )
#endif
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GDKApplication::~GDKApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKApplication::initializeOptionsService_()
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

            CHAR utf_arg[1024 + 1] = {'\0'};
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
    bool GDKApplication::initializeLoggerService_()
    {
        if( LOGGER_SERVICE()
            ->isSilent() == true )
        {
            return true;
        }

        bool OPTION_console = HAS_OPTION( "console" );

        if( OPTION_console == true )
        {
            Win32ConsoleLoggerPtr loggerConsole = Helper::makeFactorableUnique<Win32ConsoleLogger>( MENGINE_DOCUMENT_FUNCTION );

            loggerConsole->setVerboseLevel( LM_INFO );
            loggerConsole->setWriteHistory( true );

            if( LOGGER_SERVICE()
                ->registerLogger( loggerConsole ) == true )
            {
                m_loggerConsole = loggerConsole;
            }
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
        loggerOutputDebug->setWriteHistory( true );

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
            extraFileLogger->setWriteHistory( true );

            if( LOGGER_SERVICE()
                ->registerLogger( extraFileLogger ) == true )
            {
                m_loggerExtraFile = extraFileLogger;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKApplication::finalizeLoggerService_()
    {
        if( m_loggerConsole != nullptr )
        {
            if( SERVICE_IS_INITIALIZE( LoggerServiceInterface ) == true )
            {
                LOGGER_SERVICE()
                    ->unregisterLogger( m_loggerConsole );
            }

            m_loggerConsole = nullptr;
        }

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
    bool GDKApplication::initialize()
    {
#if defined(MENGINE_PLUGIN_MENGINE_SHARED)
        HINSTANCE hInstance = ::LoadLibrary( MENGINE_DLL_NAME );

        if( hInstance == NULL )
        {
            DWORD error_code = ::GetLastError();

            MENGINE_UNUSED( error_code );

            WChar message[1024 + 1] = {L'\0'};
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

            WChar message[1024 + 1] = {L'\0'};
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

        UNKNOWN_SERVICE_WAIT( GDKApplication, OptionsServiceInterface, [this]()
        {
            if( this->initializeOptionsService_() == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_WAIT( GDKApplication, LoggerServiceInterface, [this]()
        {
            if( this->initializeLoggerService_() == false )
            {
                return false;
            }

            return true;
        } );

        UNKNOWN_SERVICE_LEAVE( GDKApplication, LoggerServiceInterface, [this]()
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

        ConstString Window_ApplicationTitleTextId = CONFIG_VALUE_CONSTSTRING( "Window", "ApplicationTitleTextId", STRINGIZE_STRING_LOCAL( "APPLICATION_TITLE" ) );

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

        bool fullscreen = APPLICATION_SERVICE()
            ->getFullscreenMode();

        Resolution windowResolution;
        if( APPLICATION_SERVICE()
            ->calcWindowResolution( fullscreen, &windowResolution ) == false )
        {
            LOGGER_FATAL( "invalid calculate window resolution for fullscreen %s"
                , fullscreen == true ? "YES" : "NO"
            );

            return false;
        }

        if( PLATFORM_SERVICE()
            ->createWindow( windowResolution, fullscreen ) == false )
        {
            LOGGER_FATAL( "invalid create window %u:%u fullscreen %s"
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

#if defined(MENGINE_LOGGER_INFO_ENABLE)
        Timestamp runPlatformTimeMilliseconds = PLATFORM_SERVICE()
            ->getPlatfomTime();

        Timestamp runPlatformTimeSeconds = runPlatformTimeMilliseconds / 1000;

        LOGGER_INFO( "system", "run platform time: %" MENGINE_PRIu64 " seconds"
            , runPlatformTimeSeconds
        );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKApplication::loop()
    {
        PLATFORM_SERVICE()
            ->loopPlatform();
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKApplication::finalize()
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