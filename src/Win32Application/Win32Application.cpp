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
#include "Interface/EnumeratorServiceInterface.h"
#include "Interface/ChronometerServiceInterface.h"
#include "Interface/ModuleServiceInterface.h"
#include "Interface/FrameworkInterface.h"
#include "Interface/PluginServiceInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/EasingServiceInterface.h"
#include "Interface/PlayerServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/FactoryDefault.h"
#include "Kernel/StringArguments.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/FileLogger.h"
#include "Kernel/IniUtil.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "MessageBoxLogger.h"

#include <cstdio>
#include <clocale>
#include <memory>
#include <errno.h>
#include <ctime>
#include <algorithm>
#include <functional>

#include "resource.h"

#ifndef MENGINE_SETLOCALE
#define MENGINE_SETLOCALE "C"
#endif

#ifndef MENGINE_APPLICATION_INI_PATH
#define MENGINE_APPLICATION_INI_PATH "application.ini"
#endif

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
SERVICE_EXTERN( ModuleService );
SERVICE_EXTERN( CodecService );
SERVICE_EXTERN( DataService );
SERVICE_EXTERN( PrefetcherService );
SERVICE_EXTERN( MemoryService );
SERVICE_EXTERN( ConverterService );
SERVICE_EXTERN( InputService );
SERVICE_EXTERN( TimeSystem );
SERVICE_EXTERN( PluginService );
SERVICE_EXTERN( EasingService );
SERVICE_EXTERN( PrototypeService );
SERVICE_EXTERN( VocabularyService );
SERVICE_EXTERN( UpdateService );
SERVICE_EXTERN( LoaderService );
SERVICE_EXTERN( RenderService );
SERVICE_EXTERN( RenderMaterialService );
SERVICE_EXTERN( RenderTextureService );
SERVICE_EXTERN( ResourceService );
SERVICE_EXTERN( SceneService );
SERVICE_EXTERN( TextService );
SERVICE_EXTERN( WatchdogService );
SERVICE_EXTERN( GraveyardService );
SERVICE_EXTERN( PackageService );
SERVICE_EXTERN( UserdataService );
SERVICE_EXTERN( PickerService );
SERVICE_EXTERN( PlayerService );
SERVICE_EXTERN( AccountService );
SERVICE_EXTERN( GameService );
SERVICE_EXTERN( TimelineService );
SERVICE_EXTERN( Application );
SERVICE_EXTERN( EnumeratorService );
SERVICE_EXTERN( ChronometerService );

PLUGIN_EXPORT( ImageCodec );
PLUGIN_EXPORT( SoundCodec );
PLUGIN_EXPORT( Amplifier );
PLUGIN_EXPORT( Zip );
PLUGIN_EXPORT( LZ4 );
PLUGIN_EXPORT( OggVorbis );
PLUGIN_EXPORT( Win32FileGroup );

#ifdef MENGINE_PLUGIN_BITMAPFONT_STATIC
PLUGIN_EXPORT( BitmapFont );
#endif

#ifdef MENGINE_PLUGIN_NODEDEBUGRENDER_STATIC
PLUGIN_EXPORT( NodeDebugRender );
#endif

#ifdef MENGINE_PLUGIN_NODELEAKDETECTOR_STATIC
PLUGIN_EXPORT( NodeLeakDetector );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEPREFETCHER_STATIC
PLUGIN_EXPORT( ResourcePrefetcher );
#endif

#ifdef MENGINE_PLUGIN_RESOURCECONVERT_STATIC
PLUGIN_EXPORT( ResourceConvert );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEVALIDATE_STATIC
PLUGIN_EXPORT( ResourceValidate );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEDEBUGGER_STATIC
PLUGIN_EXPORT( ResourceDebugger );
#endif

#ifdef MENGINE_PLUGIN_METABUFLOADER_STATIC
PLUGIN_EXPORT( MetabufLoader );
#endif

#ifdef MENGINE_PLUGIN_WEBP_STATIC
PLUGIN_EXPORT( WebP );
#endif

#ifdef MENGINE_PLUGIN_VIDEO_STATIC
PLUGIN_EXPORT( Video );
#endif

#ifdef MENGINE_PLUGIN_THEORA_STATIC
PLUGIN_EXPORT( Theora );
#endif

#ifdef MENGINE_PLUGIN_CURL_STATIC
PLUGIN_EXPORT( cURL );
#endif

#ifdef MENGINE_PLUGIN_SPINE_STATIC
PLUGIN_EXPORT( Spine );
#endif

#ifdef MENGINE_PLUGIN_MOVIE_STATIC
PLUGIN_EXPORT( Movie );
#endif

#ifdef MENGINE_PLUGIN_MOVIE1_STATIC
PLUGIN_EXPORT( Movie1 );
#endif

#ifdef MENGINE_PLUGIN_TTF_STATIC
PLUGIN_EXPORT( TTF );
#endif

#ifdef MENGINE_PLUGIN_WIN32_SOCKET_STATIC
PLUGIN_EXPORT( Win32Socket );
#endif

#ifdef MENGINE_PLUGIN_NODEDEBUGGER_STATIC
PLUGIN_EXPORT( NodeDebugger );
#endif

#ifdef MENGINE_PLUGIN_OZZ_ANIMATION_STATIC
PLUGIN_EXPORT( OzzAnimation );
#endif

#ifdef MENGINE_PLUGIN_PYTHONFRAMEWORK_STATIC
PLUGIN_EXPORT( PythonFramework );
#endif

#ifdef MENGINE_PLUGIN_JSON_STATIC
PLUGIN_EXPORT( JSON );
#endif

#ifdef MENGINE_PLUGIN_TEXTUREPACKER_STATIC
PLUGIN_EXPORT( Texturepacker );
#endif

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

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( fileGroup, applicationPath, false, MENGINE_DOCUMENT_FUNCTION );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "Invalid open application settings %s"
                , applicationPath.c_str()
            );

            return false;
        }

        IniUtil::IniStore ini;
        if( IniUtil::loadIni( ini, stream ) == false )
        {
            LOGGER_ERROR( "Invalid load application settings %s"
                , applicationPath.c_str()
            );

            return false;
        }

        FilePath publicConfigPath;
        if( IniUtil::getIniValue( ini, "Config", "Public", publicConfigPath ) == true ||
            IniUtil::getIniValue( ini, "Game", "Path", publicConfigPath ) == true )
        {
            m_publicConfigPath = publicConfigPath;
        }

        FilePath privateConfigPath;
        if( IniUtil::getIniValue( ini, "Config", "Private", privateConfigPath ) == true )
        {
            m_privateConfigPath = privateConfigPath;
        }

        FilePath resourcePath;
        if( IniUtil::getIniValue( ini, "Resource", "Path", resourcePath ) == true )
        {
            m_resourceConfigPath = resourcePath;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::initializeConfigService_()
    {
        LOGGER_MESSAGE( "Inititalizing Config Manager..." );

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        if( CONFIG_SERVICE()
            ->loadConfig( fileGroup, m_publicConfigPath, m_privateConfigPath ) == false )
        {
            LOGGER_ERROR( "invalid load config %s"
                , m_publicConfigPath.c_str()
            );

            return false;
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
        if( FILE_SERVICE()->mountFileGroup( ConstString::none(), nullptr, FilePath( ConstString::none() ), STRINGIZE_STRING_LOCAL( "dir" ), nullptr, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            LOGGER_ERROR( "failed to mount application directory '%s'"
                , currentPath
            );

            return false;
        }

#	ifndef MENGINE_MASTER_RELEASE
        // mount root		
        if( FILE_SERVICE()->mountFileGroup( STRINGIZE_STRING_LOCAL( "dev" ), nullptr, FilePath( ConstString::none() ), STRINGIZE_STRING_LOCAL( "global" ), nullptr, MENGINE_DOCUMENT_FUNCTION ) == false )
        {
            LOGGER_ERROR( "failed to mount dev directory '%s'"
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
        Char userPath[MENGINE_MAX_PATH] = {0};
        size_t userPathLen = PLATFORM_SERVICE()
            ->getUserPath( userPath );

        // mount user directory
        if( FILE_SERVICE()
            ->mountFileGroup( STRINGIZE_STRING_LOCAL( "user" ), nullptr, Helper::stringizeFilePathSize( userPath, userPathLen ), STRINGIZE_STRING_LOCAL( "global" ), nullptr, MENGINE_DOCUMENT_FUNCTION ) == false )
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
    bool Win32Application::initializeLogFile_()
    {
        bool nologs = HAS_OPTION( "nologs" );

        if( nologs == true )
        {
            return true;
        }

        Char date[1024] = {0};
        PLATFORM_SERVICE()
            ->makeDateTime( date, 1024 );

        WString unicode_date;
        Helper::utf8ToUnicode( date, unicode_date );

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

        OutputStreamInterfacePtr fileLogInterface = FILE_SERVICE()
            ->openOutputFile( fileGroup, logFilename, MENGINE_DOCUMENT_FUNCTION );

        if( fileLogInterface != nullptr )
        {
            m_fileLog = Helper::makeFactorableUnique<FileLogger>();

            LOGGER_SERVICE()
                ->registerLogger( m_fileLog );

            LOGGER_INFO( "WinApplication: Starting log to %s"
                , logFilename.c_str()
            );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::initializeLogService_()
    {
        if( LOGGER_SERVICE()
            ->isSilent() == true )
        {
            return true;
        }

        m_loggerMessageBox = Helper::makeFactorableUnique<MessageBoxLogger>();

        m_loggerMessageBox->setVerboseLevel( LM_CRITICAL );

        LOGGER_SERVICE()
            ->registerLogger( m_loggerMessageBox );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32Application::initialize()
    {
        stdex_allocator_initialize();

        ::setlocale( LC_ALL, MENGINE_SETLOCALE );
        
        ServiceProviderInterface * serviceProvider;
        SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider );

        SERVICE_PROVIDER_SETUP( serviceProvider );

        m_serviceProvider = serviceProvider;

        SERVICE_CREATE( FactoryService );
        SERVICE_CREATE( UnicodeSystem );
        SERVICE_CREATE( OptionsService );

        if( this->initializeOptionsService_() == false )
        {
            return false;
        }

        SERVICE_CREATE( NotificationService );
        SERVICE_CREATE( StringizeService );
        SERVICE_CREATE( VocabularyService );
        SERVICE_CREATE( LoggerService );

        if( this->initializeLogService_() == false )
        {
            return false;
        }

        SERVICE_CREATE( Platform );

        SERVICE_CREATE( PluginService );
        SERVICE_CREATE( FileService );

        if( this->initializeFileService_() == false )
        {
            return false;
        }

        if( this->loadApplicationConfig_() == false )
        {
            return false;
        }

        SERVICE_CREATE( ConfigService );

        if( this->initializeConfigService_() == false )
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

        m_mutexAllocatorPool = THREAD_SERVICE()
            ->createMutex( MENGINE_DOCUMENT_FUNCTION );

        stdex_allocator_initialize_threadsafe( m_mutexAllocatorPool.get()
            , (stdex_allocator_thread_lock_t)&s_stdex_thread_lock
            , (stdex_allocator_thread_unlock_t)&s_stdex_thread_unlock
        );

        SERVICE_CREATE( PrototypeService );

        SERVICE_CREATE( RenderSystem );
        SERVICE_CREATE_SAFE( SoundSystem );

        bool muteMode = HAS_OPTION( "mute" );

        if( muteMode == true || SERVICE_EXIST( Mengine::SoundSystemInterface ) == false )
        {
            SERVICE_CREATE( SilentSoundSystem );
        }

        SERVICE_CREATE( SoundService );

        SERVICE_CREATE( ModuleService );
        SERVICE_CREATE( CodecService );
        SERVICE_CREATE( DataService );
        SERVICE_CREATE( PrefetcherService );
        SERVICE_CREATE( MemoryService );
        SERVICE_CREATE( ConverterService );
        SERVICE_CREATE( InputService );
        SERVICE_CREATE( EnumeratorService );
        SERVICE_CREATE( ChronometerService );
        SERVICE_CREATE( TimeSystem );
        SERVICE_CREATE( EasingService );



        SERVICE_CREATE( UpdateService );
        SERVICE_CREATE( LoaderService );

        SERVICE_CREATE( RenderService );
        SERVICE_CREATE( RenderMaterialService );
        SERVICE_CREATE( RenderTextureService );

        SERVICE_CREATE( SceneService );
        SERVICE_CREATE( ResourceService );
        SERVICE_CREATE( TextService );
        SERVICE_CREATE( WatchdogService );
        SERVICE_CREATE( GraveyardService );
        SERVICE_CREATE( PackageService );
        SERVICE_CREATE( UserdataService );
        SERVICE_CREATE( PickerService );
        SERVICE_CREATE( PlayerService );
        SERVICE_CREATE( AccountService );
        SERVICE_CREATE( GameService );
        SERVICE_CREATE( TimelineService );
        SERVICE_CREATE( EnumeratorService );

        LOGGER_MESSAGE( "initialize Plugins..." );

#	define MENGINE_ADD_PLUGIN( Name, Info )\
		{LOGGER_INFO( Info );\
		if(	PLUGIN_CREATE(Name) == false ){\
		LOGGER_ERROR( "Invalid %s", Info );}else{\
		LOGGER_MESSAGE( "Successful %s", Info );}}

#ifdef MENGINE_PLUGIN_PYTHONFRAMEWORK_STATIC
        MENGINE_ADD_PLUGIN( PythonFramework, "initialize Plugin PythonFramework..." );
#endif

#ifdef MENGINE_PLUGIN_NODEDEBUGRENDER_STATIC
        MENGINE_ADD_PLUGIN( NodeDebugRender, "initialize Plugin Node Debug Render..." );
#endif

#ifdef MENGINE_PLUGIN_NODELEAKDETECTOR_STATIC
        MENGINE_ADD_PLUGIN( NodeLeakDetector, "initialize Plugin Node Leak Detector..." );
#endif        

#ifdef MENGINE_PLUGIN_RESOURCEPREFETCHER_STATIC
        MENGINE_ADD_PLUGIN( ResourcePrefetcher, "initialize Plugin Resource Prefetcher..." );
#endif

#ifdef MENGINE_PLUGIN_RESOURCECONVERT_STATIC
        MENGINE_ADD_PLUGIN( ResourceConvert, "initialize Plugin Resource Convert..." );
#endif        

#ifdef MENGINE_PLUGIN_RESOURCEVALIDATE_STATIC
        MENGINE_ADD_PLUGIN( ResourceValidate, "initialize Plugin Resource Validate..." );
#endif

#ifdef MENGINE_PLUGIN_RESOURCEDEBUGGER_STATIC
        MENGINE_ADD_PLUGIN( ResourceDebugger, "initialize Plugin Resource Debugger..." );
#endif

#ifdef MENGINE_PLUGIN_NODEDEBUGGER_STATIC
        MENGINE_ADD_PLUGIN( NodeDebugger, "initialize Plugin NodeDebugger..." );
#endif

#ifdef MENGINE_PLUGIN_OZZ_ANIMATION_STATIC
        MENGINE_ADD_PLUGIN( OzzAnimation, "initialize Plugin OzzAnimation..." );
#endif

#ifdef MENGINE_PLUGIN_JSON_STATIC
        MENGINE_ADD_PLUGIN( JSON, "initialize Plugin JSON..." );
#endif

#ifdef MENGINE_PLUGIN_TEXTUREPACKER_STATIC
        MENGINE_ADD_PLUGIN( Texturepacker, "initialize Plugin Texturepacker..." );
#endif

#ifdef MENGINE_PLUGIN_METABUFLOADER_STATIC
        MENGINE_ADD_PLUGIN( MetabufLoader, "initialize Plugin Metabuf Loader..." );
#endif

        MENGINE_ADD_PLUGIN( ImageCodec, "initialize Plugin Image Codec..." );
        MENGINE_ADD_PLUGIN( SoundCodec, "initialize Plugin Sound Codec..." );
        MENGINE_ADD_PLUGIN( OggVorbis, "initialize Plugin Ogg Vorbis Codec..." );
        MENGINE_ADD_PLUGIN( Amplifier, "initialize Plugin Amplifier..." );

#ifdef MENGINE_PLUGIN_WEBP_STATIC
        MENGINE_ADD_PLUGIN( WebP, "initialize Plugin WebP..." );
#endif

#ifdef MENGINE_PLUGIN_VIDEO_STATIC
        MENGINE_ADD_PLUGIN( Video, "initialize Plugin Video..." );
#endif

#ifdef MENGINE_PLUGIN_THEORA_STATIC
        MENGINE_ADD_PLUGIN( Theora, "initialize Plugin Theora..." );
#endif

#ifdef MENGINE_PLUGIN_CURL_STATIC
        MENGINE_ADD_PLUGIN( cURL, "initialize Plugin cURL..." );
#endif

#ifdef MENGINE_PLUGIN_SPINE_STATIC
        MENGINE_ADD_PLUGIN( Spine, "initialize Plugin Spine..." );
#endif

#ifdef MENGINE_PLUGIN_MOVIE_STATIC
        MENGINE_ADD_PLUGIN( Movie, "initialize Plugin Movie..." );
#endif

#ifdef MENGINE_PLUGIN_MOVIE1_STATIC
        MENGINE_ADD_PLUGIN( Movie1, "initialize Plugin Movie1..." );
#endif
        //MENGINE_ADD_PLUGIN(Motor, "initialize Plugin Motor...");
        //MENGINE_ADD_PLUGIN( Box2D, "initialize Plugin Box2D..." );
        //MENGINE_ADD_PLUGIN( PathFinder, "initialize Plugin Path Finder..." );

#ifdef MENGINE_PLUGIN_BITMAPFONT_STATIC
        MENGINE_ADD_PLUGIN( BitmapFont, "initialize Plugin Bitmap Font..." );
#endif

#ifdef MENGINE_PLUGIN_TTF_STATIC
        MENGINE_ADD_PLUGIN( TTF, "initialize Plugin TTF..." );
#endif

#ifdef MENGINE_PLUGIN_WIN32_SOCKET_STATIC
        MENGINE_ADD_PLUGIN( Win32Socket, "initialize Plugin Win32Socket..." );
#endif

#	undef MENGINE_ADD_PLUGIN

        VectorString plugins;
        CONFIG_VALUES( "Plugins", "Name", plugins );

        for( const String & pluginName : plugins )
        {
            if( PLUGIN_SERVICE()
                ->loadPlugin( pluginName.c_str() ) == false )
            {
                LOGGER_CRITICAL( "Application Failed to load plugin '%s'"
                    , pluginName.c_str()
                );

                return false;
            }
        }

#ifdef MENGINE_MASTER_RELEASE
        bool devplugins = false;
        bool devmodules = false;
#else
#ifdef MENGINE_DEBUG
        bool devplugins = true;
        bool devmodules = true;
#else
        bool developmentMode = HAS_OPTION( "dev" );
        bool devplugins = developmentMode;
        bool devmodules = developmentMode;
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
                    LOGGER_WARNING( "Application Failed to load dev plugin '%s'"
                        , pluginName.c_str()
                    );
                }
            }
        }

        SERVICE_CREATE( Application );

        LOGGER_MESSAGE( "Modules Run..." );

        VectorString modules;
        CONFIG_VALUES( "Modules", "Name", modules );

        for( const String & moduleName : modules )
        {
            if( MODULE_SERVICE()
                ->runModule( Helper::stringizeString( moduleName ), MENGINE_DOCUMENT_FUNCTION ) == false )
            {
                LOGGER_CRITICAL( "Application Failed to run module '%s'"
                    , moduleName.c_str()
                );
            }
        }

        bool nodevmodules = HAS_OPTION( "nodevmodules" );

        if( devmodules == true && nodevmodules == false )
        {
            VectorString devModules;
            CONFIG_VALUES( "DevModules", "Name", devModules );

            for( const String & moduleName : devModules )
            {
                if( MODULE_SERVICE()
                    ->runModule( Helper::stringizeString( moduleName ), MENGINE_DOCUMENT_FUNCTION ) == false )
                {
                    LOGGER_ERROR( "Application Failed to run dev module '%s'"
                        , moduleName.c_str()
                    );
                }
            }
        }

        FilePath renderMaterialsPath = CONFIG_VALUE( "Engine", "RenderMaterials", FilePath( ConstString::none() ) );

        if( renderMaterialsPath.empty() == false )
        {
            LOGGER_MESSAGE( "Materials Load..." );

            const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
                ->getDefaultFileGroup();

            if( RENDERMATERIAL_SERVICE()
                ->loadMaterials( fileGroup, renderMaterialsPath ) == false )
            {
                return false;
            }
        }

        LOGGER_MESSAGE( "Application Create..." );

        const FileGroupInterfacePtr & fileGroup = FILE_SERVICE()
            ->getDefaultFileGroup();

        if( APPLICATION_SERVICE()
            ->initializeGame( fileGroup, m_resourceConfigPath ) == false )
        {
            LOGGER_CRITICAL( "Application invalid initialize game"
            );

            return false;
        }

        FRAMEWORK_SERVICE()
            ->onFrameworkInitialize();

        if( GAME_SERVICE()
            ->loadPersonality() == false )
        {
            LOGGER_CRITICAL( "Game invalid load personality"
            );

            return false;
        }

        LOGGER_MESSAGE( "Creating Render Window..." );

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
            ->runPlatform();

        PLATFORM_SERVICE()
            ->updatePlatform();
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
			NOTIFICATION_NOTIFY( NOTIFICATOR_ENGINE_FINALIZE );
        }

        SERVICE_PROVIDER_STOP();

        if( SERVICE_EXIST( Mengine::ThreadServiceInterface ) == true )
        {
            THREAD_SERVICE()
                ->stopTasks();
        }

        SERVICE_FINALIZE( Mengine::AccountServiceInterface );
        SERVICE_FINALIZE( Mengine::SceneServiceInterface );
        SERVICE_FINALIZE( Mengine::GameServiceInterface );
        SERVICE_FINALIZE( Mengine::PlayerServiceInterface );
        SERVICE_FINALIZE( Mengine::PickerServiceInterface );
        SERVICE_FINALIZE( Mengine::ChronometerServiceInterface );
        SERVICE_FINALIZE( Mengine::UpdateServiceInterface );
        SERVICE_FINALIZE( Mengine::PrototypeServiceInterface );
        SERVICE_FINALIZE( Mengine::PrefetcherServiceInterface );
        SERVICE_FINALIZE( Mengine::ApplicationInterface );
        SERVICE_FINALIZE( Mengine::PackageServiceInterface );
        SERVICE_FINALIZE( Mengine::UserdataServiceInterface );
        SERVICE_FINALIZE( Mengine::GraveyardInterface );
        SERVICE_FINALIZE( Mengine::ResourceServiceInterface );
        SERVICE_FINALIZE( Mengine::TextServiceInterface );
        SERVICE_FINALIZE( Mengine::DataServiceInterface );

        PLUGIN_SERVICE()
            ->unloadPlugins();

        SERVICE_FINALIZE( Mengine::InputServiceInterface );
        SERVICE_FINALIZE( Mengine::UnicodeSystemInterface );

        SERVICE_FINALIZE( Mengine::CodecServiceInterface );

        SERVICE_FINALIZE( Mengine::SoundServiceInterface );
        SERVICE_FINALIZE( Mengine::SoundSystemInterface );
                
        SERVICE_FINALIZE( Mengine::ConverterServiceInterface );

        SERVICE_FINALIZE( Mengine::RenderServiceInterface );
        SERVICE_FINALIZE( Mengine::RenderMaterialServiceInterface );
        SERVICE_FINALIZE( Mengine::RenderTextureServiceInterface );
        SERVICE_FINALIZE( Mengine::RenderSystemInterface );

        SERVICE_FINALIZE( Mengine::ConfigServiceInterface );
        SERVICE_FINALIZE( Mengine::StringizeServiceInterface );

        SERVICE_FINALIZE( Mengine::ArchiveServiceInterface );
        SERVICE_FINALIZE( Mengine::MemoryServiceInterface );

        SERVICE_FINALIZE( Mengine::ThreadServiceInterface );

        SERVICE_FINALIZE( Mengine::TimeSystemInterface );

        SERVICE_FINALIZE( Mengine::EasingServiceInterface );
        SERVICE_FINALIZE( Mengine::VocabularyServiceInterface );
        SERVICE_FINALIZE( Mengine::EnumeratorServiceInterface );

        SERVICE_FINALIZE( Mengine::PlatformInterface );

        SERVICE_FINALIZE( Mengine::PluginServiceInterface );

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
        SERVICE_FINALIZE( Mengine::OptionsServiceInterface );
        SERVICE_FINALIZE( Mengine::NotificationServiceInterface );
        SERVICE_FINALIZE( Mengine::LoggerServiceInterface );

        stdex_allocator_finalize_threadsafe();

        m_mutexAllocatorPool = nullptr;

        SERVICE_PROVIDER_FINALIZE( m_serviceProvider );

        stdex_allocator_finalize();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Application::update()
    {
        //
    }
    //////////////////////////////////////////////////////////////////////////
}
