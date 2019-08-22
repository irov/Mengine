#include "Config/Config.h"
#include "Config/Stringstream.h"

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
#include "Kernel/Document.h"
#include "Kernel/FileLogger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/UnicodeHelper.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "MessageBoxLogger.h"

#include <cstdio>
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

#ifndef MENGINE_APPLICATION_INI_PATH
#define MENGINE_APPLICATION_INI_PATH "application.ini"
#endif

SERVICE_PROVIDER_EXTERN( ServiceProvider );

SERVICE_EXTERN( Bootstrapper );

PLUGIN_EXPORT( Win32FileGroup );

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
    Win32Application::Win32Application()
        : m_serviceProvider( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32Application::~Win32Application()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::loadApplicationConfig_()
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
            ->loadConfig( fileGroup, applicationPath, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( config, false, "invalid open application settings '%s'"
            , applicationPath.c_str()
        );

        config->getValues( "Game", "Path", m_configPaths );
        config->getValues( "Config", "Path", m_configPaths );
        config->getValues( "Credential", "Path", m_configPaths );
        config->getValues( "Resource", "Path", m_resourceConfigPaths );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::initializeConfigService_()
    {
        LOGGER_MESSAGE( "Inititalizing Config Manager..." );

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        for( const FilePath & filePath : m_configPaths )
        {
            if( CONFIG_SERVICE()
                ->loadDefaultConfig( fileGroup, filePath, MENGINE_DOCUMENT_FUNCTION ) == false )
            {
                LOGGER_ERROR( "invalid load config '%s'"
                    , filePath.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::initializeFileService_()
    {
        LOGGER_INFO( "Inititalizing File Service..." );

        {
            LOGGER_INFO( "Initialize Win32 file group..." );
            PLUGIN_CREATE( Win32FileGroup );
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

        // mount root		
        if( FILE_SERVICE()
            ->mountFileGroup( ConstString::none(), nullptr, FilePath( ConstString::none() ), STRINGIZE_STRING_LOCAL( "dir" ), nullptr, false, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            LOGGER_ERROR( "failed to mount application directory '%s'"
                , currentPath
            );

            return false;
        }

#ifndef MENGINE_MASTER_RELEASE
        // mount root		
        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ), nullptr, FilePath( ConstString::none() ), STRINGIZE_STRING_LOCAL( "global" ), nullptr, false, MENGINE_DOCUMENT_FUNCTION ) == false )
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
    bool Win32Application::initializeUserDirectory_()
    {
        Char userPath[MENGINE_MAX_PATH] = { 0 };
        size_t userPathLen = PLATFORM_SERVICE()
            ->getUserPath( userPath );

        // mount user directory
        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "user" ), nullptr, Helper::stringizeFilePathSize( userPath, userPathLen ), STRINGIZE_STRING_LOCAL( "global" ), nullptr, true, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            LOGGER_ERROR( "failed to mount user directory '%s'"
                , userPath
            );

            return false;
        }

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

        ArgumentsInterfacePtr arguments = Helper::makeFactorableUnique<StringArguments>();

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
    bool Win32Application::initializeLoggerFile_()
    {
        bool nologs = HAS_OPTION( "nologs" );

        if( nologs == true )
        {
            return true;
        }

        DateTimeProviderInterfacePtr dateTimeProvider = PLATFORM_SERVICE()
            ->createDateTimeProvider( MENGINE_DOCUMENT_FUNCTION );

        PlatformDateTime dateTime;
        dateTimeProvider->getDateTime( &dateTime );

        Stringstream ss_date;
        ss_date << dateTime.year
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << (dateTime.month)
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.day
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.hour
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.minute
            << "_" << std::setw( 2 ) << std::setfill( '0' ) << dateTime.second;

        String str_date = ss_date.str();

        WString unicode_date;
        Helper::utf8ToUnicode( str_date, unicode_date );

        WString unicode_logFilename;
        unicode_logFilename += L"Game";

        bool developmentMode = HAS_OPTION( "dev" );
        bool roamingMode = HAS_OPTION( "roaming" );
        bool noroamingMode = HAS_OPTION( "noroaming" );

        if( developmentMode == true && roamingMode == false || noroamingMode == false )
        {
            unicode_logFilename += L"_";
            unicode_logFilename += unicode_date;
        }

        unicode_logFilename += L".log";

        String utf8_logFilename;
        if( Helper::unicodeToUtf8( unicode_logFilename, utf8_logFilename ) == false )
        {
            LOGGER_ERROR( "failed log directory '%ls' convert to ut8f"
                , unicode_logFilename.c_str()
            );

            return false;
        }

        FilePath logFilename = Helper::stringizeFilePath( utf8_logFilename );

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        FileLoggerPtr fileLog = Helper::makeFactorableUnique<FileLogger>();

        fileLog->setFileGroup( fileGroup );
        fileLog->setFilePath( logFilename );

        if( LOGGER_SERVICE()
            ->registerLogger( fileLog ) == false )
        {
            LOGGER_ERROR( "invalid register file logger '%s'"
                , logFilename.c_str()
            );
        }
        else
        {
            m_fileLog = fileLog;

            LOGGER_INFO( "starting log to '%s'"
                , logFilename.c_str()
            );
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

        MessageBoxLoggerPtr loggerMessageBox = Helper::makeFactorableUnique<MessageBoxLogger>();

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

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        if( APPLICATION_SERVICE()
            ->initializeGame( fileGroup, m_resourceConfigPaths ) == false )
        {
            LOGGER_CRITICAL( "invalid initialize Game"
            );

            return false;
        }

        if( SERVICE_EXIST( FrameworkInterface ) == false )
        {
            LOGGER_CRITICAL( "invalid found Framework"
            );

            return false;
        }

        if( FRAMEWORK_SERVICE()
            ->onFrameworkInitialize() == false )
        {
            LOGGER_CRITICAL( "invalid initialize Framework"
            );

            return false;
        }

        if( GAME_SERVICE()
            ->loadPersonality() == false )
        {
            LOGGER_CRITICAL( "invalid load Personality"
            );

            return false;
        }

        LOGGER_MESSAGE( "Creating Render Window..." );

        const Char * projectTitle = nullptr;
        size_t projectTitleLen = 0;

        TextEntryInterfacePtr entry;
        if( TEXT_SERVICE()
            ->hasTextEntry( STRINGIZE_STRING_LOCAL( "APPLICATION_TITLE" ), &entry ) == false )
        {
            LOGGER_INFO( "not setup title 'APPLICATION_TITLE'"
            );
        }
        else
        {   
            projectTitle = entry->getValue( &projectTitleLen );
        }

        PLATFORM_SERVICE()
            ->setProjectTitle( projectTitle, projectTitleLen );

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
        SERVICE_LEAVE( FileServiceInterface, [this]()
        {
            if( m_fileLog != nullptr )
            {
                if( SERVICE_EXIST( LoggerServiceInterface ) == true )
                {
                    LOGGER_SERVICE()
                        ->unregisterLogger( m_fileLog );
                }

                m_fileLog = nullptr;
            }
        } );

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
