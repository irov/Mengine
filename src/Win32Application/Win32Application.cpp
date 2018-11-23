#include "Config/Config.h"
#include "Config/Stringstream.h"

#include "Win32Application.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/OptionsInterface.h"
#include "Interface/LoggerInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/PackageServiceInterface.h"
#include "Interface/UserdataServiceInterface.h"
#include "Interface/GraveyardInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/UpdateServiceInterface.h"
#include "Interface/ThreadServiceInterface.h"
#include "Interface/DataServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/MemoryServiceInterface.h"
#include "Interface/ConverterServiceInterface.h"
#include "Interface/AccountServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/FactoryDefault.h"

#include "Kernel/Date.h"

#include "Kernel/Logger.h"

#include "PythonScriptWrapper/PythonWrapper.h"

#include <cstdio>
#include <clocale>
#include <memory>

#include <errno.h>

#include "MessageBoxLogger.h"

#include "CriticalErrorsMonitor.h"

#include "Kernel/FileLogger.h"
#include "Kernel/IniUtil.h"

#include "resource.h"

#include <ctime>
#include <algorithm>
#include <functional>

#include "Environment/Windows/WindowsIncluder.h"

SERVICE_PROVIDER_EXTERN( ServiceProvider );

SERVICE_EXTERN( FactoryService );
SERVICE_EXTERN( OptionsService );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( LoggerService );
SERVICE_EXTERN( Platform );
SERVICE_EXTERN( NotificationService );
SERVICE_EXTERN( UnicodeSystem );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( ConfigService );
SERVICE_EXTERN( ArchiveService );
SERVICE_EXTERN( ThreadService );
SERVICE_EXTERN( ThreadSystem );
SERVICE_EXTERN( RenderSystem );
SERVICE_EXTERN( SoundService );
SERVICE_EXTERN( SoundSystem );
SERVICE_EXTERN( SilentSoundSystem );
SERVICE_EXTERN( ScriptService );
SERVICE_EXTERN( ModuleService );
SERVICE_EXTERN( CodecService );
SERVICE_EXTERN( DataService );
SERVICE_EXTERN( PrefetcherService );
SERVICE_EXTERN( MemoryService );
SERVICE_EXTERN( ConverterService );
SERVICE_EXTERN( InputService );
SERVICE_EXTERN( TimeService );
SERVICE_EXTERN( TimeSystem );
SERVICE_EXTERN( PluginSystem );
SERVICE_EXTERN( PluginService );
SERVICE_EXTERN( PrototypeService );
SERVICE_EXTERN( UpdateService );
SERVICE_EXTERN( LoaderService );
SERVICE_EXTERN( RenderService );
SERVICE_EXTERN( RenderMaterialService );
SERVICE_EXTERN( RenderTextureService );
SERVICE_EXTERN( ResourceService );
SERVICE_EXTERN( TextService );
SERVICE_EXTERN( WatchdogService );
SERVICE_EXTERN( ProfilerService );
SERVICE_EXTERN( GraveyardService );
SERVICE_EXTERN( PackageService );
SERVICE_EXTERN( UserdataService );
SERVICE_EXTERN( PickerService )
SERVICE_EXTERN( PlayerService );
SERVICE_EXTERN( AccountService );
SERVICE_EXTERN( GameService );
SERVICE_EXTERN( TimelineService );
SERVICE_EXTERN( Application );

PLUGIN_EXPORT( ImageCodec );
PLUGIN_EXPORT( SoundCodec );
PLUGIN_EXPORT( Amplifier );
PLUGIN_EXPORT( Zip );
PLUGIN_EXPORT( LZ4 );
PLUGIN_EXPORT( OggVorbis );
PLUGIN_EXPORT( Win32FileGroup );

#ifdef MENGINE_PLUGIN_BITMAPFONT
    PLUGIN_EXPORT( BitmapFont );
#endif

#ifdef MENGINE_PLUGIN_NODEDEBUGRENDER
    PLUGIN_EXPORT( NodeDebugRender );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEPREFETCHER
    PLUGIN_EXPORT( ResourcePrefetcher );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEVALIDATE
    PLUGIN_EXPORT( ResourceValidate );
#endif

#ifdef MENGINE_PLUGIN_METABUFLOADER
    PLUGIN_EXPORT( MetabufLoader );
#endif

#ifdef MENGINE_PLUGIN_VIDEO
PLUGIN_EXPORT( Video );
#endif

#ifdef MENGINE_PLUGIN_THEORA
PLUGIN_EXPORT( Theora );
#endif

#ifdef MENGINE_PLUGIN_CURL_NO_DLL
PLUGIN_EXPORT( cURL );
#endif

#ifdef MENGINE_PLUGIN_SPINE
PLUGIN_EXPORT( Spine );
#endif

PLUGIN_EXPORT( Movie );
PLUGIN_EXPORT( Movie1 );
//PLUGIN_EXPORT( Box2D );
//PLUGIN_EXPORT( PathFinder );

#ifdef MENGINE_PLUGIN_TTF
PLUGIN_EXPORT( TTF );
#endif

PLUGIN_EXPORT( Win32Socket );
PLUGIN_EXPORT( NodeDebugger );

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
    bool Win32Application::initializeArchiveService_()
    {
        {
            LOGGER_INFO( "Initialize Zip..." );
            PLUGIN_CREATE( Zip );
        }

        {
            LOGGER_INFO( "Initialize LZ4..." );
            PLUGIN_CREATE( LZ4 );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::getApplicationPath_( const char * _section, const char * _key, ConstString & _path )
    {
        FilePath applicationPath = STRINGIZE_FILEPATH_LOCAL( "application.ini" );

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        InputStreamInterfacePtr applicationInputStream = FILE_SERVICE()
            ->openInputFile( fileGroup, applicationPath, false );

        if( applicationInputStream == nullptr )
        {
            LOGGER_ERROR( "WinApplication::initializeConfigEngine_ Invalid open application settings %s"
                , applicationPath.c_str()
            );

            return false;
        }

        std::unique_ptr<IniUtil::IniStore> ini( new IniUtil::IniStore() );
        if( IniUtil::loadIni( *ini.get(), applicationInputStream ) == false )
        {
            LOGGER_ERROR( "WinApplication::initializeConfigEngine_ Invalid load application settings %s"
                , applicationPath.c_str()
            );

            return false;
        }

        const char * gameIniPath = ini->getSettingValue( _section, _key );

        if( gameIniPath == nullptr )
        {
            LOGGER_ERROR( "WinApplication::initializeConfigEngine_ Not found Game Path %s"
                , applicationPath.c_str()
            );

            return false;
        }

        _path = Helper::stringizeString( gameIniPath );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::initializeConfigEngine_()
    {
        LOGGER_WARNING( "Inititalizing Config Manager..." );

        FilePath gameIniPath;
        if( this->getApplicationPath_( "Game", "Path", gameIniPath ) == false )
        {
            return false;
        }

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        if( CONFIG_SERVICE()
            ->loadConfig( fileGroup, gameIniPath ) == false )
        {
            LOGGER_ERROR( "WinApplication::initializeConfigEngine_ invalid load config %s"
                , gameIniPath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::initializeFileEngine_()
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
            LOGGER_ERROR( "WinApplication::setupFileService: failed to get current directory"
            );

            return false;
        }

        LOGGER_WARNING( "Current Path '%s'"
            , currentPath
        );

        // mount root		
        if( FILE_SERVICE()->mountFileGroup( ConstString::none(), nullptr, FilePath( ConstString::none() ), STRINGIZE_STRING_LOCAL( "dir" ), nullptr ) == false )
        {
            LOGGER_ERROR( "WinApplication::setupFileService: failed to mount application directory %ls"
                , currentPath
            );

            return false;
        }

#	ifndef MENGINE_MASTER_RELEASE
        // mount root		
        if( FILE_SERVICE()->mountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ), nullptr, FilePath( ConstString::none() ), STRINGIZE_STRING_LOCAL( "global" ), nullptr ) == false )
        {
            LOGGER_ERROR( "WinApplication::setupFileService: failed to mount dev directory %ls"
                , currentPath
            );

            return false;
        }
#   endif

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
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "user" ), nullptr, Helper::stringizeFilePath( userPath, userPathLen ), STRINGIZE_STRING_LOCAL( "global" ), nullptr ) == false )
        {
            LOGGER_ERROR( "WinApplication: failed to mount user directory %ls"
                , userPath
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::initializeLogFile_()
    {
        bool nologs = HAS_OPTION( "nologs" );

        if( nologs == true )
        {
            return true;
        }

        WString date;
        Helper::makeDateTimeW( date );

        WString unicode_logFilename;
        unicode_logFilename += L"Game";

        bool developmentMode = HAS_OPTION( "dev" );
        bool roamingMode = HAS_OPTION( "roaming" );
        bool noroamingMode = HAS_OPTION( "noroaming" );

        if( developmentMode == true && roamingMode == false || noroamingMode == false )
        {
            unicode_logFilename += L"_";
            unicode_logFilename += date;
        }

        unicode_logFilename += L".log";

        String utf8_logFilename;
        if( Helper::unicodeToUtf8( unicode_logFilename, utf8_logFilename ) == false )
        {
            LOGGER_ERROR( "WinApplication: failed log directory %ls convert to ut8f"
                , unicode_logFilename.c_str()
            );

            return false;
        }

        FilePath logFilename = Helper::stringizeFilePath( utf8_logFilename );

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        OutputStreamInterfacePtr fileLogInterface = FILE_SERVICE()
            ->openOutputFile( fileGroup, logFilename );

        if( fileLogInterface != nullptr )
        {
            m_fileLog = new FactorableUnique<FileLogger>();

            LOGGER_SERVICE()
                ->registerLogger( m_fileLog );

            LOGGER_INFO( "WinApplication: Starting log to %s"
                , logFilename.c_str()
            );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::initializeLogEngine_()
    {
        bool nologs = HAS_OPTION( "nologs" );

        if( nologs == false )
        {
            m_loggerMessageBox = new FactorableUnique<MessageBoxLogger>();

            m_loggerMessageBox->setVerboseLevel( LM_CRITICAL );

            LOGGER_SERVICE()
                ->registerLogger( m_loggerMessageBox );
        }

        EMessageLevel m_logLevel;

        bool developmentMode = HAS_OPTION( "dev" );
        bool roamingMode = HAS_OPTION( "roaming" );
        bool noroamingMode = HAS_OPTION( "noroaming" );

        if( developmentMode == true && roamingMode == false || noroamingMode == true )
        {
            m_logLevel = LM_LOG;
        }
        else
        {
            m_logLevel = LM_ERROR;
        }

        if( TEST_OPTION_VALUE( "log", "0" ) == true )
        {
            m_logLevel = LM_INFO;
        }
        else if( TEST_OPTION_VALUE( "log", "1" ) == true )
        {
            m_logLevel = LM_LOG;
        }
        else if( TEST_OPTION_VALUE( "log", "2" ) == true )
        {
            m_logLevel = LM_WARNING;
        }
        else if( TEST_OPTION_VALUE( "log", "3" ) == true )
        {
            m_logLevel = LM_ERROR;
        }
        else if( TEST_OPTION_VALUE( "log", "4" ) == true )
        {
            m_logLevel = LM_CRITICAL;
        }
        else if( TEST_OPTION_VALUE( "log", "5" ) == true )
        {
            m_logLevel = LM_FATAL;
        }

        LOGGER_SERVICE()
            ->setVerboseLevel( m_logLevel );

        uint32_t verboseFlag = 0;

        bool profiler = HAS_OPTION( "profiler" );

        if( profiler == true )
        {
            verboseFlag |= 0x00000001;
        }

        LOGGER_SERVICE()
            ->setVerboseFlag( verboseFlag );

        if( HAS_OPTION( "verbose" ) == true )
        {
            LOGGER_SERVICE()
                ->setVerboseLevel( LM_MAX );

            LOGGER_INFO( "Verbose logging mode enabled" );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::initialize()
    {
        setlocale( LC_ALL, "C" );
        //::timeBeginPeriod( 1 );

        ServiceProviderInterface * serviceProvider;
        SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider );

        SERVICE_PROVIDER_SETUP( serviceProvider );

        m_serviceProvider = serviceProvider;

        SERVICE_CREATE( FactoryService );

        SERVICE_CREATE( OptionsService );

        LPCWSTR lpCmdLine = GetCommandLineW();

        if( lpCmdLine == NULL )
        {
            return false;
        }

        int pNumArgs;
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

        VectorString args;
        for( int i = 1; i != pNumArgs; ++i )
        {
            PWSTR arg = szArglist[i];

            CHAR utf_arg[1024];

            int utf_arg_size = ::WideCharToMultiByte(
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

            args.push_back( utf_arg );
        }
        LocalFree( szArglist );

        OPTIONS_SERVICE()
            ->setArgs( args );

        SERVICE_CREATE( StringizeService );
        SERVICE_CREATE( LoggerService );

        if( this->initializeLogEngine_() == false )
        {
            return false;
        }

        SERVICE_CREATE( Platform );

        SERVICE_CREATE( UnicodeSystem );

        SERVICE_CREATE( PluginSystem );
        SERVICE_CREATE( PluginService );

        SERVICE_CREATE( FileService );

        if( this->initializeFileEngine_() == false )
        {
            return false;
        }

        SERVICE_CREATE( ConfigService );

        if( this->initializeConfigEngine_() == false )
        {
            return false;
        }

        if( this->initializeUserDirectory_() == false )
        {
            return false;
        }

        if( this->initializeLogFile_() == false )
        {
            return false;
        }

        SERVICE_CREATE( ArchiveService );

        if( this->initializeArchiveService_() == false )
        {
            return false;
        }

        SERVICE_CREATE( ThreadSystem );
        SERVICE_CREATE( ThreadService );

        ThreadMutexInterfacePtr threadMutexForLogger = THREAD_SERVICE()
            ->createMutex( __FILE__, __LINE__ );

        LOGGER_SERVICE()
            ->setThreadMutex( threadMutexForLogger );

        SERVICE_CREATE( NotificationService );

        SERVICE_CREATE( RenderSystem );


        SERVICE_CREATE( SoundSystem );

        bool muteMode = HAS_OPTION( "mute" );

        if( muteMode == true || SERVICE_EXIST( Mengine::SoundSystemInterface ) == false )
        {
            SERVICE_CREATE( SilentSoundSystem );
        }

        SERVICE_CREATE( SoundService );

        SERVICE_CREATE( ScriptService );
        SERVICE_CREATE( ModuleService );
        SERVICE_CREATE( CodecService );
        SERVICE_CREATE( DataService );
        SERVICE_CREATE( PrefetcherService );
        SERVICE_CREATE( MemoryService );
        SERVICE_CREATE( ConverterService );
        SERVICE_CREATE( InputService );

        SERVICE_CREATE( TimeSystem );
        SERVICE_CREATE( TimeService );

#ifndef NDEBUG
        {
            bool developmentMode = HAS_OPTION( "dev" );
            bool roamingMode = HAS_OPTION( "roaming" );
            bool noroamingMode = HAS_OPTION( "noroaming" );

            if( developmentMode == true && (roamingMode == false || noroamingMode == true) )
            {
                Char userPath[MENGINE_MAX_PATH] = { 0 };
                PLATFORM_SERVICE()
                    ->getUserPath( userPath );

                CriticalErrorsMonitor::run( userPath );
            }
        }
#endif

        SERVICE_CREATE( PrototypeService );
        SERVICE_CREATE( UpdateService );
        SERVICE_CREATE( LoaderService );

        if( PythonWrapper::constsWrap() == false )
        {
            return false;
        }

        if( PythonWrapper::helperWrap() == false )
        {
            return false;
        }

        if( PythonWrapper::nodeWrap() == false )
        {
            return false;
        }

        if( PythonWrapper::mathWrap() == false )
        {
            return false;
        }

        if( PythonWrapper::entityWrap() == false )
        {
            return false;
        }

        if( PythonWrapper::engineWrap() == false )
        {
            return false;
        }

        if( PythonWrapper::soundWrap() == false )
        {
            return false;
        }

        SERVICE_CREATE( RenderService );
        SERVICE_CREATE( RenderMaterialService );
        SERVICE_CREATE( RenderTextureService );

        SERVICE_CREATE( ResourceService );
        SERVICE_CREATE( TextService );
        SERVICE_CREATE( WatchdogService );
        SERVICE_CREATE( ProfilerService );
        SERVICE_CREATE( GraveyardService );
        SERVICE_CREATE( PackageService );
        SERVICE_CREATE( UserdataService );
        SERVICE_CREATE( PickerService );
        SERVICE_CREATE( PlayerService );
        SERVICE_CREATE( AccountService );
        SERVICE_CREATE( GameService );
        SERVICE_CREATE( TimelineService );

        SERVICE_CREATE( Application );

        // seed randomizer
        LARGE_INTEGER randomSeed;
        ::QueryPerformanceCounter( &randomSeed );
        srand( randomSeed.LowPart );

        LOGGER_WARNING( "initialize Plugins..." );

#	define MENGINE_ADD_PLUGIN( Name, Info )\
		do{LOGGER_INFO( Info );\
		if(	PLUGIN_CREATE(Name) == false ){\
		LOGGER_ERROR( "Invalid %s", Info );}else{\
		LOGGER_WARNING( "Successful %s", Info );}}while(false, false)

#ifdef MENGINE_PLUGIN_NODEDEBUGRENDER
        MENGINE_ADD_PLUGIN( NodeDebugRender, "initialize Plugin Node Debug Render..." );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEPREFETCHER
        MENGINE_ADD_PLUGIN( ResourcePrefetcher, "initialize Plugin Resource Prefetcher..." );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEVALIDATE
        MENGINE_ADD_PLUGIN( ResourceValidate, "initialize Plugin Resource Validate..." );
#endif

#ifdef MENGINE_PLUGIN_METABUFLOADER
        MENGINE_ADD_PLUGIN( MetabufLoader, "initialize Plugin Metabuf Loader..." );
#endif

        MENGINE_ADD_PLUGIN( ImageCodec, "initialize Plugin Image Codec..." );
        MENGINE_ADD_PLUGIN( SoundCodec, "initialize Plugin Sound Codec..." );
        MENGINE_ADD_PLUGIN( OggVorbis, "initialize Plugin Ogg Vorbis Codec..." );
        MENGINE_ADD_PLUGIN( Amplifier, "initialize Plugin Amplifier..." );

#ifdef MENGINE_PLUGIN_VIDEO
        MENGINE_ADD_PLUGIN( Video, "initialize Plugin Video..." );
#endif

#ifdef MENGINE_PLUGIN_THEORA
        MENGINE_ADD_PLUGIN( Theora, "initialize Plugin Theora..." );
#endif

#ifdef MENGINE_PLUGIN_CURL_NO_DLL
        MENGINE_ADD_PLUGIN( cURL, "initialize Plugin cURL..." );
#endif

#ifdef MENGINE_PLUGIN_SPINE
        MENGINE_ADD_PLUGIN( Spine, "initialize Plugin Spine..." );
#endif
        MENGINE_ADD_PLUGIN( Movie, "initialize Plugin Movie..." );
        MENGINE_ADD_PLUGIN( Movie1, "initialize Plugin Movie1..." );
        //MENGINE_ADD_PLUGIN(Motor, "initialize Plugin Motor...");
        //MENGINE_ADD_PLUGIN( Box2D, "initialize Plugin Box2D..." );
        //MENGINE_ADD_PLUGIN( PathFinder, "initialize Plugin Path Finder..." );

#ifdef MENGINE_PLUGIN_BITMAPFONT
        MENGINE_ADD_PLUGIN( BitmapFont, "initialize Plugin Bitmap Font..." );
#endif

#ifdef MENGINE_PLUGIN_TTF
        MENGINE_ADD_PLUGIN( TTF, "initialize Plugin TTF..." );
#endif

        MENGINE_ADD_PLUGIN( Win32Socket, "initialize Plugin Win32Socket..." );
        MENGINE_ADD_PLUGIN( NodeDebugger, "initialize Plugin NodeDebugger..." );

#	undef MENGINE_ADD_PLUGIN

        VectorString plugins;
        CONFIG_VALUES( "Plugins", "Name", plugins );

        for( const String & pluginName : plugins )
        {
            if( PLUGIN_SERVICE()
                ->loadPlugin( pluginName.c_str() ) == false )
            {
                LOGGER_CRITICAL( "Application Failed to load plugin %ls"
                    , pluginName.c_str()
                );

                return false;
            }
        }

#ifdef MENGINE_MASTER_RELEASE
        bool devplugins = false;
#else
#ifndef NDEBUG
        bool devplugins = true;
#else
        bool developmentMode = HAS_OPTION( "dev" );
        bool devplugins = developmentMode;
#endif
#endif

        bool nodevplugins = HAS_OPTION( "nodevplugins" );

        if( devplugins == true && nodevplugins == false )
        {
            VectorString devPlugins;
            CONFIG_VALUES( "DevPlugins", "Name", devPlugins );

            for( const String & pluginName : devPlugins )
            {
                if( PLUGIN_SERVICE()
                    ->loadPlugin( pluginName.c_str() ) == false )
                {
                    LOGGER_WARNING( "Application Failed to load dev plugin %ls"
                        , pluginName.c_str()
                    );
                }
            }
        }

        LOGGER_WARNING( "Modules Run..." );

        VectorString modules;
        CONFIG_VALUES( "Modules", "Name", modules );

        for( const String & moduleName : modules )
        {
            if( MODULE_SERVICE()
                ->runModule( Helper::stringizeString( moduleName ) ) == false )
            {
                LOGGER_CRITICAL( "Application Failed to run module %s"
                    , moduleName.c_str()
                );
            }
        }

        FilePath renderMaterialsPath = CONFIG_VALUE( "Engine", "RenderMaterials", FilePath( ConstString::none() ) );

        if( renderMaterialsPath.empty() == false )
        {
            LOGGER_WARNING( "Materials Load..."
            );

            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getDefaultFileGroup();

            if( RENDERMATERIAL_SERVICE()
                ->loadMaterials( fileGroup, renderMaterialsPath ) == false )
            {
                return false;
            }
        }

        LOGGER_WARNING( "Application Create..."
        );

        FilePath resourceIniPath;
        if( this->getApplicationPath_( "Resource", "Path", resourceIniPath ) == false )
        {
            LOGGER_CRITICAL( "Application invalid setup resource path"
            );

            return false;
        }

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        if( APPLICATION_SERVICE()
            ->initializeGame( fileGroup, resourceIniPath ) == false )
        {
            LOGGER_CRITICAL( "Application invalid initialize game"
            );

            return false;
        }

        if( PythonWrapper::gameWrap() == false )
        {
            return false;
        }

        if( APPLICATION_SERVICE()
            ->loadPersonality() == false )
        {
            LOGGER_CRITICAL( "Application invalid initialize game"
            );

            return false;
        }

        LOGGER_WARNING( "Creating Render Window..." );

        bool fullscreen = APPLICATION_SERVICE()
            ->getFullscreenMode();

        String projectTitle;

        TextEntryInterfacePtr entry;
        if( TEXT_SERVICE()
            ->existText( STRINGIZE_STRING_LOCAL( "APPLICATION_TITLE" ), &entry ) == false )
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

        Resolution windowResolution;
        APPLICATION_SERVICE()
            ->calcWindowResolution( windowResolution );

        PLATFORM_SERVICE()
            ->setIcon( IDI_MENGINE );

        if( PLATFORM_SERVICE()
            ->createWindow( windowResolution, fullscreen ) == false )
        {
            return false;
        }

        //mt::vec2f point;
        //this->calcCursorPosition( point );

        //INPUT_SERVICE()
        //	->setCursorPosition( 0, point );

        if( APPLICATION_SERVICE()
            ->createRenderWindow() == false )
        {
            LOGGER_CRITICAL( "Application not create render window"
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
            ->update();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Application::finalize()
    {
        SERVICE_FINALIZE( Mengine::ModuleServiceInterface );

        if( SERVICE_EXIST( Mengine::PlatformInterface ) == true )
        {
            PLATFORM_SERVICE()
                ->stopPlatform();
        }

        if( SERVICE_EXIST( Mengine::NotificationServiceInterface ) == true )
        {
            NOTIFICATION_SERVICE()
                ->notify( NOTIFICATOR_ENGINE_FINALIZE );
        }

        SERVICE_PROVIDER_STOP();

        if( SERVICE_EXIST( Mengine::ThreadServiceInterface ) == true )
        {
            THREAD_SERVICE()
                ->stopTasks();
        }

        SERVICE_FINALIZE( Mengine::AccountServiceInterface );
        SERVICE_FINALIZE( Mengine::GameServiceInterface );
        SERVICE_FINALIZE( Mengine::PickerServiceInterface );
        SERVICE_FINALIZE( Mengine::PlayerServiceInterface );
        SERVICE_FINALIZE( Mengine::PackageServiceInterface );
        SERVICE_FINALIZE( Mengine::UserdataServiceInterface );
        SERVICE_FINALIZE( Mengine::GraveyardInterface );
        SERVICE_FINALIZE( Mengine::UpdateServiceInterface );
        SERVICE_FINALIZE( Mengine::ResourceServiceInterface );
        SERVICE_FINALIZE( Mengine::TextServiceInterface );
        SERVICE_FINALIZE( Mengine::PrototypeServiceInterface );
        SERVICE_FINALIZE( Mengine::ApplicationInterface );
        SERVICE_FINALIZE( Mengine::PrefetcherServiceInterface );
        SERVICE_FINALIZE( Mengine::DataServiceInterface );
        SERVICE_FINALIZE( Mengine::PluginServiceInterface );
        SERVICE_FINALIZE( Mengine::InputServiceInterface );
        SERVICE_FINALIZE( Mengine::UnicodeSystemInterface );

        SERVICE_FINALIZE( Mengine::CodecServiceInterface );

        SERVICE_FINALIZE( Mengine::SoundServiceInterface );
        SERVICE_FINALIZE( Mengine::SoundSystemInterface );

        SERVICE_FINALIZE( Mengine::PrototypeServiceInterface );
        SERVICE_FINALIZE( Mengine::ScriptServiceInterface );
        SERVICE_FINALIZE( Mengine::ConverterServiceInterface );

        SERVICE_FINALIZE( Mengine::RenderServiceInterface );
        SERVICE_FINALIZE( Mengine::RenderMaterialServiceInterface );
        SERVICE_FINALIZE( Mengine::RenderTextureServiceInterface );
        SERVICE_FINALIZE( Mengine::RenderSystemInterface );

        SERVICE_FINALIZE( Mengine::ConfigServiceInterface );
        SERVICE_FINALIZE( Mengine::StringizeServiceInterface );

        SERVICE_FINALIZE( Mengine::ArchiveServiceInterface );
        SERVICE_FINALIZE( Mengine::MemoryServiceInterface );
        SERVICE_FINALIZE( Mengine::NotificationServiceInterface );

        SERVICE_FINALIZE( Mengine::ThreadServiceInterface );

        SERVICE_FINALIZE( Mengine::TimeServiceInterface );
        SERVICE_FINALIZE( Mengine::TimeSystemInterface );

        SERVICE_FINALIZE( Mengine::PlatformInterface );

        if( m_fileLog != nullptr )
        {
            if( SERVICE_EXIST( Mengine::LoggerServiceInterface ) == true )
            {
                LOGGER_SERVICE()
                    ->unregisterLogger( m_fileLog );
            }

            m_fileLog = nullptr;
        }

        if( m_loggerMessageBox != nullptr )
        {
            if( SERVICE_EXIST( Mengine::LoggerServiceInterface ) == true )
            {
                LOGGER_SERVICE()
                    ->unregisterLogger( m_loggerMessageBox );
            }

            m_loggerMessageBox = nullptr;
        }

        SERVICE_FINALIZE( Mengine::FileServiceInterface );
        SERVICE_FINALIZE( Mengine::ThreadSystemInterface );
        SERVICE_FINALIZE( Mengine::LoggerServiceInterface );

        SERVICE_PROVIDER_FINALIZE( m_serviceProvider );

        //::timeEndPeriod( 1 );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Application::update()
    {
        //
    }
    //////////////////////////////////////////////////////////////////////////
}
