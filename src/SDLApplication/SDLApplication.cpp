#	include "Config/Config.h"

#	include "SDLApplication.h"

#	include "Menge/Application.h"

#	include "Interface/LoggerInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/InputSystemInterface.h"
#	include "Interface/TimerInterface.h"
#	include "Interface/OptionsInterface.h"

#   include "PythonScriptWrapper/PythonScriptWrapper.h"

#	include <cstdio>
#	include <clocale>

#	include "Factory/FactorableUnique.h"
#	include "Factory/FactoryDefault.h"

#	include "Core/FileLogger.h"
#	include "Core/IniUtil.h"
#	include "Core/Date.h"

#   include "SDLMessageBoxLogger.h"
#   include "SDLStdioLogger.h"

//#	include "resource.h"

#	include <ctime>
#	include <algorithm>

#	include <sstream>
#	include <iomanip>

#   include "SDL_filesystem.h"

#ifdef WIN32
#	ifdef _WIN32_WINNT	
#       undef _WIN32_WINNT
#       define _WIN32_WINNT 0x0500
#   endif

#   ifdef _WIN32_WINDOWS
#       undef _WIN32_WINDOWS
#       define _WIN32_WINDOWS 0x0500
#   endif

#	define WIN32_LEAN_AND_MEAN

#	ifndef NOMINMAX
#	define NOMINMAX
#	endif

#pragma warning(push, 0) 
#	include <Windows.h>
#   include <WinUser.h>

#   include <shellapi.h>
#   include <shlobj.h>
#pragma warning(pop) 
#endif

#ifdef _MSC_VER
#	define snprintf _snprintf
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_PROVIDER_EXTERN( ServiceProvider );
//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( FactoryService );
SERVICE_EXTERN( SDLLayer );
SERVICE_EXTERN( Platform );
SERVICE_EXTERN( Application );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( LoggerService );

SERVICE_EXTERN( ArchiveService );

SERVICE_EXTERN( ThreadSystem );
SERVICE_EXTERN( ThreadService );

SERVICE_EXTERN( ParticleService );

SERVICE_EXTERN( RenderSystem );
SERVICE_EXTERN( RenderService );
SERVICE_EXTERN( RenderTextureService );
SERVICE_EXTERN( RenderMaterialService );

SERVICE_EXTERN( PhysicSystem );

SERVICE_EXTERN( UnicodeSystem );
SERVICE_EXTERN( UnicodeService );

SERVICE_EXTERN( FileService );

SERVICE_EXTERN( NotificationService );
SERVICE_EXTERN( ScriptService );

SERVICE_EXTERN( SoundSystem );
SERVICE_EXTERN( SilentSoundSystem );
SERVICE_EXTERN( SoundService );

SERVICE_EXTERN( InputService );
SERVICE_EXTERN( CodecService );
SERVICE_EXTERN( PluginSystem );
SERVICE_EXTERN( PluginService );

SERVICE_EXTERN( ModuleService );
SERVICE_EXTERN( DataService );
SERVICE_EXTERN( MemoryService );
SERVICE_EXTERN( ConverterService );
SERVICE_EXTERN( ConfigService );
SERVICE_EXTERN( PrefetcherService );
SERVICE_EXTERN( OptionsSystem );
SERVICE_EXTERN( OptionsService );
SERVICE_EXTERN( TimerSystem );
SERVICE_EXTERN( TimerService );
SERVICE_EXTERN( HttpSystem );
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT( MengeImageCodec );
PLUGIN_EXPORT( MengeSoundCodec );
PLUGIN_EXPORT( MengeVideoCodec );
PLUGIN_EXPORT( MengeAmplifier );
PLUGIN_EXPORT( MengeZip );
PLUGIN_EXPORT( MengeLZ4 );
#ifdef MENGINE_PLUGIN_SPINE
PLUGIN_EXPORT( Spine );
#endif
PLUGIN_EXPORT( Movie );
PLUGIN_EXPORT( Box2D );
PLUGIN_EXPORT( MengeOggVorbis );
PLUGIN_EXPORT( PathFinder );
PLUGIN_EXPORT( MengeSDLFileGroup );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    SDLApplication::SDLApplication()
        : m_serviceProvider(nullptr)
        , m_running(false)
        , m_active(false)
        , m_developmentMode(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLApplication::~SDLApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeFileEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Inititalizing File Service..." );
        SERVICE_CREATE( m_serviceProvider, FileService );

        LOGGER_INFO( m_serviceProvider )("Initialize SDL file group...");
        PLUGIN_CREATE( m_serviceProvider, MengeSDLFileGroup );
        
#	ifdef _MSC_VER
        WChar currentPathW[MENGINE_MAX_PATH];
		size_t currentPathW_len = PLATFORM_SERVICE(m_serviceProvider)
			->getCurrentPath(currentPathW, MENGINE_MAX_PATH);

        Char utf8_currentPath[MENGINE_MAX_PATH];
        size_t utf8_currentPath_len;
        UNICODE_SERVICE( m_serviceProvider )
            ->unicodeToUtf8( currentPathW, currentPathW_len + 1, utf8_currentPath, MENGINE_MAX_PATH, &utf8_currentPath_len );

        //FilePath currentPath = Helper::stringizeStringSize( m_serviceProvider, utf8_currentPath, utf8_currentPath_len );
		FilePath currentPath;
#   else
        //char * basePath = SDL_GetBasePath();

        //FilePath currentPath = Helper::stringizeString( m_serviceProvider, basePath );

        //SDL_free( basePath );
        FilePath currentPath;
#	endif
        
        // mount root		
        ConstString c_dir = Helper::stringizeString(m_serviceProvider, "dir");
        if( FILE_SERVICE( m_serviceProvider )
            ->mountFileGroup( ConstString::none(), currentPath, c_dir ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "SDLApplication::setupFileService: failed to mount application directory %ls"
                , currentPath.c_str()
                );

            return false;
        }

#	ifndef MENGINE_MASTER_RELEASE
        ConstString c_dev = Helper::stringizeString( m_serviceProvider, "dev" );
        // mount root
        if( FILE_SERVICE(m_serviceProvider)
            ->mountFileGroup( c_dev, FilePath(ConstString::none()), c_dir ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "SDLApplication::setupFileService: failed to mount dev directory %ls"
                , currentPath.c_str()
                );

            return false;
        }
#   endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::getApplicationPath_( const char * _section, const char * _key, ConstString & _path )
    {
        FilePath applicationPath = STRINGIZE_FILEPATH_LOCAL( m_serviceProvider, "application.ini" );

        IniUtil::IniStore ini;
        if( IniUtil::loadIni( ini, ConstString::none(), applicationPath, m_serviceProvider ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLApplication::initializeConfigEngine_ Invalid load application settings %s"
                , applicationPath.c_str()
                );

            return false;
        }

        const char * gameIniPath = ini.getSettingValue( _section, _key );

        if( gameIniPath == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLApplication::initializeConfigEngine_ Not found Game Path %s"
                , applicationPath.c_str()
                );

            return false;
        }

        _path = Helper::stringizeString( m_serviceProvider, gameIniPath );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeConfigEngine_()
    {
        LOGGER_WARNING(m_serviceProvider)("Inititalizing Config Manager..." );
        SERVICE_CREATE( m_serviceProvider, ConfigService );

        FilePath gameIniPath;
        if( this->getApplicationPath_( "Game", "Path", gameIniPath ) == false )
        {
            return false;
        }

        if( CONFIG_SERVICE(m_serviceProvider)
            ->loadConfig( ConstString::none(), gameIniPath ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLApplication::initializeConfigEngine_ invalid load config %s"
                , gameIniPath.c_str()
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeUserDirectory_()
    {
		WChar userPathW[MENGINE_MAX_PATH];
		size_t userPathLen = PLATFORM_SERVICE( m_serviceProvider )
			->getUserPath( userPathW, MENGINE_MAX_PATH );

		Char utf8_userPath[MENGINE_MAX_PATH];
		size_t utf8_userPathLen;
		UNICODE_SERVICE( m_serviceProvider )
			->unicodeToUtf8( userPathW, userPathLen, utf8_userPath, MENGINE_MAX_PATH, &utf8_userPathLen );

		FilePath cs_userPath = Helper::stringizeFilePath(m_serviceProvider, utf8_userPath, utf8_userPathLen );
		
        // mount user directory
        if( FILE_SERVICE( m_serviceProvider )
            ->mountFileGroup( Helper::stringizeString( m_serviceProvider, "user" ), cs_userPath, Helper::stringizeString( m_serviceProvider, "dir" ) ) == false )
        {
            LOGGER_ERROR( m_serviceProvider )("SDLApplication: failed to mount user directory %ls"
                , cs_userPath.c_str()
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeLoggerFile_()
    {
        bool nologs = HAS_OPTION( m_serviceProvider, "nologs" );

        if( nologs == true )
        {
            return true;
        }

        m_fileLog = new FactorableUnique<FileLogger>();

        LOGGER_SERVICE( m_serviceProvider )
            ->registerLogger( m_fileLog );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeLoggerEngine_()
    {
        SERVICE_CREATE( m_serviceProvider, LoggerService );

        bool nologs = HAS_OPTION( m_serviceProvider, "nologs" );

        if( nologs == true )
        {
            return true;
        }

		m_loggerStdio = new FactorableUnique<SDLStdioLogger>();

		m_loggerStdio->setVerboseFlag( LM_LOG );

		LOGGER_SERVICE( m_serviceProvider )
			->registerLogger( m_loggerStdio );

        m_loggerMessageBox = new FactorableUnique<SDLMessageBoxLogger>();

        m_loggerMessageBox->setVerboseLevel( LM_CRITICAL );

        LOGGER_SERVICE(m_serviceProvider)
            ->registerLogger( m_loggerMessageBox );

        EMessageLevel m_logLevel;

        bool developmentMode = HAS_OPTION( m_serviceProvider, "dev" );

        if( developmentMode == true  )
        {
            m_logLevel = LM_LOG;
        }
        else
        {
            m_logLevel = LM_ERROR;
        }        

        const Char * option_log = GET_OPTION_VALUE( m_serviceProvider, "log" );
        
        if( option_log != nullptr )
        {
            uint32_t option_log_value;
            if( sscanf( option_log, "%u", &option_log_value ) == 1 )
            {
                switch( option_log_value )
                {
                case 0:
                    {
                        m_logLevel = LM_INFO;
                    }break;
                case 1:
                    {
                        m_logLevel = LM_LOG;
                    }break;
                case 2:
                    {
                        m_logLevel = LM_WARNING;
                    }break;
                case 3:
                    {
                        m_logLevel = LM_ERROR;
                    }break;
                }
            }
        }

        LOGGER_SERVICE(m_serviceProvider)
            ->setVerboseLevel( m_logLevel );

        if( HAS_OPTION( m_serviceProvider, "verbose" ) == true )
        {
            LOGGER_SERVICE(m_serviceProvider)
                ->setVerboseLevel( LM_MAX );

            LOGGER_INFO(m_serviceProvider)( "Verbose logging mode enabled" );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeArchiveService_()
    {
        LOGGER_INFO(m_serviceProvider)( "Inititalizing Archive Service..." );

        {
            LOGGER_INFO(m_serviceProvider)( "initialize Zip..." );
            
            PLUGIN_CREATE( m_serviceProvider, MengeZip );
        }

        {
            LOGGER_INFO(m_serviceProvider)( "initialize LZ4..." );

            PLUGIN_CREATE( m_serviceProvider, MengeLZ4 );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeRenderEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Render Service..." );

        if( SERVICE_CREATE( m_serviceProvider, RenderSystem ) == false )
        {
            return false;
        }

        if( SERVICE_CREATE( m_serviceProvider, RenderTextureService ) == false )
        {
            return false;
        }

        if( SERVICE_CREATE( m_serviceProvider, RenderMaterialService ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeSoundEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Sound Service..." );

        SERVICE_CREATE( m_serviceProvider, SoundSystem );

        bool muteMode = HAS_OPTION( m_serviceProvider, "mute" );

        if( muteMode == true || SERVICE_EXIST( m_serviceProvider, Menge::SoundSystemInterface ) == false )
        {
            SERVICE_CREATE( m_serviceProvider, SilentSoundSystem );
        }

        if( SERVICE_CREATE( m_serviceProvider, SoundService ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("SDLApplication::initializeSoundEngine_ Failed to create Sound Engine"
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initialize(const int argc, char** argv)
    {
        setlocale( LC_ALL, "C" );

        ServiceProviderInterface * serviceProvider;
        if( SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider ) == false )
        {
            return false;
        }

        m_serviceProvider = serviceProvider;

        SERVICE_CREATE( m_serviceProvider, FactoryService );

        SERVICE_CREATE( m_serviceProvider, OptionsService );

		TVectorString options;

		for( int i = 1; i < argc; ++i )
		{
			const char * arg = argv[i];

			options.push_back( arg );
		}

		OPTIONS_SERVICE( m_serviceProvider )
			->setArgs( options );

        SERVICE_CREATE( m_serviceProvider, StringizeService );

        if( this->initializeLoggerEngine_() == false )
        {
            return false;
        }

        SERVICE_CREATE( m_serviceProvider, UnicodeSystem );
        SERVICE_CREATE( m_serviceProvider, UnicodeService );

        SERVICE_CREATE( m_serviceProvider, SDLLayer );
        SERVICE_CREATE( m_serviceProvider, Platform );

        SERVICE_CREATE( m_serviceProvider, PluginSystem );
        SERVICE_CREATE( m_serviceProvider, PluginService );

        if( this->initializeFileEngine_() == false )
        {
            return false;
        }

        if( this->initializeConfigEngine_() == false )
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


        SERVICE_CREATE( m_serviceProvider, ArchiveService );

        if( this->initializeArchiveService_() == false )
        {
            return false;
        }

        SERVICE_CREATE( m_serviceProvider, ThreadSystem );
        SERVICE_CREATE( m_serviceProvider, ThreadService );

        SERVICE_CREATE( m_serviceProvider, NotificationService );

        if( this->initializeRenderEngine_() == false )
        {
            return false;
        }

        if( this->initializeSoundEngine_() == false )
        {
            return false;
        }

        SERVICE_CREATE( m_serviceProvider, TimerSystem );
        SERVICE_CREATE( m_serviceProvider, TimerService );
        
        SERVICE_CREATE( m_serviceProvider, ScriptService );
        SERVICE_CREATE( m_serviceProvider, ModuleService );
		SERVICE_CREATE( m_serviceProvider, CodecService );
        SERVICE_CREATE( m_serviceProvider, DataService );
		SERVICE_CREATE( m_serviceProvider, PrefetcherService );
        SERVICE_CREATE( m_serviceProvider, MemoryService );
		SERVICE_CREATE( m_serviceProvider, ConverterService );                
        SERVICE_CREATE( m_serviceProvider, InputService );

		SERVICE_CREATE( m_serviceProvider, HttpSystem );

        PythonScriptWrapper::constsWrap( m_serviceProvider );
        PythonScriptWrapper::mathWrap( m_serviceProvider );
        PythonScriptWrapper::nodeWrap( m_serviceProvider );
        PythonScriptWrapper::helperWrap( m_serviceProvider );
        PythonScriptWrapper::soundWrap( m_serviceProvider );
        PythonScriptWrapper::entityWrap( m_serviceProvider );
        
        SERVICE_CREATE( m_serviceProvider, Application );

#   define MENGINE_ADD_PLUGIN( Name, Info )\
        do{LOGGER_INFO(m_serviceProvider)( Info );\
        if(	PLUGIN_CREATE(m_serviceProvider, Name) == false ){\
        LOGGER_ERROR(m_serviceProvider)( "Invalid %s", Info );}else{\
        LOGGER_WARNING(m_serviceProvider)( "Successful %s", Info );}}while(false, false)

        MENGINE_ADD_PLUGIN(MengeImageCodec, "initialize Plugin Image Codec...");
        MENGINE_ADD_PLUGIN(MengeSoundCodec, "initialize Plugin Sound Codec...");
        MENGINE_ADD_PLUGIN(MengeOggVorbis, "initialize Plugin Ogg Vorbis Codec...");
        MENGINE_ADD_PLUGIN(MengeAmplifier, "initialize Plugin Amplifier...");
        MENGINE_ADD_PLUGIN(MengeVideoCodec, "initialize Plugin Video Codec...");
#ifdef MENGINE_PLUGIN_SPINE
        MENGINE_ADD_PLUGIN(Spine, "initialize Plugin Spine...");
#endif
        MENGINE_ADD_PLUGIN(Movie, "initialize Plugin Movie...");
        //MENGINE_ADD_PLUGIN(Motor, "initialize Plugin Motor...");
        MENGINE_ADD_PLUGIN(Box2D, "initialize Plugin Box2D...");

        MENGINE_ADD_PLUGIN(PathFinder, "initialize Plugin Path Finder...");

#   undef MENGINE_ADD_PLUGIN

        TVectorWString plugins;
        CONFIG_VALUES(m_serviceProvider, "Plugins", "Name", plugins);

        for( TVectorWString::const_iterator
            it = plugins.begin(),
            it_end = plugins.end();
        it != it_end;
        ++it )
        {
            const WString & pluginName = *it;

            if( PLUGIN_SERVICE(m_serviceProvider)
                ->loadPlugin( pluginName ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)("Application Failed to load plugin %ls"
                    , pluginName.c_str()
                    );

                return false;
            }
        }

        bool developmentMode = HAS_OPTION( m_serviceProvider, "dev" );
        bool nodevplugins = HAS_OPTION( m_serviceProvider, "nodevplugins" );

        if( developmentMode == true && nodevplugins == false )
        {
            TVectorWString devPlugins;
            CONFIG_VALUES( m_serviceProvider, "DevPlugins", "Name", devPlugins );

            for( TVectorWString::const_iterator
                it = devPlugins.begin(),
                it_end = devPlugins.end();
                it != it_end;
                ++it )
            {
                const WString & pluginName = *it;

                if( PLUGIN_SERVICE( m_serviceProvider )
                    ->loadPlugin( pluginName ) == false )
                {
                    LOGGER_WARNING( m_serviceProvider )("Application Failed to load dev plugin %ls"
                        , pluginName.c_str()
                        );
                }
            }
        }

        SERVICE_CREATE(m_serviceProvider, ParticleService);

        TVectorString modules;
        CONFIG_VALUES(m_serviceProvider, "Modules", "Name", modules);

        for( TVectorString::const_iterator
            it = modules.begin(),
            it_end = modules.end();
        it != it_end;
        ++it )
        {
            const String & moduleName = *it;

            if( MODULE_SERVICE(m_serviceProvider)
                ->runModule( Helper::stringizeString(m_serviceProvider, moduleName) ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)("Application Failed to run module %s"
                    , moduleName.c_str()
                    );

                return false;
            }
        }

		FilePath renderMaterialsPathEmpty;
        FilePath renderMaterialsPath = CONFIG_VALUE( m_serviceProvider, "Engine", "RenderMaterials", renderMaterialsPathEmpty );

        if( renderMaterialsPath.empty() == false )
        {
            if( RENDERMATERIAL_SERVICE( m_serviceProvider )
                ->loadMaterials( ConstString::none(), renderMaterialsPath ) == false )
            {
                return false;
            }
        }

        LOGGER_INFO( m_serviceProvider )("Application Create...");

        FilePath resourceIniPath;
        if( this->getApplicationPath_( "Resource", "Path", resourceIniPath ) == false )
        {
            LOGGER_CRITICAL( m_serviceProvider )("Application invalid setup resource path"
                );

            return false;
        }

        if( APPLICATION_SERVICE( m_serviceProvider )
            ->initializeGame( ConstString::none(), resourceIniPath ) == false )
        {
            LOGGER_CRITICAL( m_serviceProvider )("Application invalid initialize game"
                );

            return false;
        }
               
        PLATFORM_SERVICE( m_serviceProvider )
            ->setIcon( 0 );

        const String & projectTitle = APPLICATION_SERVICE(m_serviceProvider)
            ->getProjectTitle();

        WString wprojectTitle;
        if( Helper::utf8ToUnicode(m_serviceProvider, projectTitle, wprojectTitle) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Application project title %s not convert to unicode"
                                            , projectTitle.c_str()
                                            );
        }

        PLATFORM_SERVICE( m_serviceProvider )
            ->setProjectTitle( wprojectTitle );

        Resolution windowResolution;
        APPLICATION_SERVICE(m_serviceProvider)
            ->calcWindowResolution(windowResolution);

        const bool fullscreen = APPLICATION_SERVICE( m_serviceProvider )
            ->getFullscreenMode();

        if( PLATFORM_SERVICE(m_serviceProvider)
            ->createWindow( windowResolution, fullscreen ) == false )
        {
            return false;
        }

        LOGGER_INFO(m_serviceProvider)( "Creating Render Window..." );

        if( APPLICATION_SERVICE( m_serviceProvider )
            ->createRenderWindow() == false )
        {
            return false;
        }

        APPLICATION_SERVICE( m_serviceProvider )
            ->turnSound( true );
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLApplication::loop()
    {
        PLATFORM_SERVICE( m_serviceProvider )
            ->update();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLApplication::finalize()
    {
        SERVICE_FINALIZE( m_serviceProvider, ApplicationInterface );
        SERVICE_FINALIZE( m_serviceProvider, PrefetcherServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, DataServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, PluginServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, ModuleServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, InputServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, UnicodeServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, UnicodeSystemInterface );

        SERVICE_FINALIZE( m_serviceProvider, FileServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, CodecServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, ParticleSystemInterface2 );
        SERVICE_FINALIZE( m_serviceProvider, ParticleServiceInterface2 );

        SERVICE_FINALIZE( m_serviceProvider, SoundServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, SoundSystemInterface );

        SERVICE_FINALIZE( m_serviceProvider, PrototypeServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, ScriptServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, ConverterServiceInterface );

        SERVICE_FINALIZE( m_serviceProvider, RenderServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, RenderMaterialServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, RenderTextureServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, RenderSystemInterface );

        SERVICE_FINALIZE( m_serviceProvider, ConfigServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, StringizeServiceInterface );

        SERVICE_FINALIZE( m_serviceProvider, ConfigServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, StringizeServiceInterface );

        SERVICE_FINALIZE( m_serviceProvider, ArchiveServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, MemoryServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, NotificationServiceInterface );

        SERVICE_FINALIZE( m_serviceProvider, ThreadServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, ThreadSystemInterface );

        SERVICE_FINALIZE( m_serviceProvider, TimerServiceInterface );
        SERVICE_FINALIZE( m_serviceProvider, TimerSystemInterface );

        SERVICE_FINALIZE( m_serviceProvider, PlatformInterface );
        
        if( m_fileLog != nullptr )
        {
            LOGGER_SERVICE( m_serviceProvider )
                ->unregisterLogger( m_fileLog );

            m_fileLog = nullptr;
        }

		if( m_loggerStdio != nullptr )
		{
			LOGGER_SERVICE( m_serviceProvider )
				->unregisterLogger( m_loggerStdio );

			m_loggerStdio = nullptr;
		}
        
        if( m_loggerMessageBox != nullptr )
        {
            LOGGER_SERVICE( m_serviceProvider )
                ->unregisterLogger( m_loggerMessageBox );

            m_loggerMessageBox = nullptr;
        }

        SERVICE_FINALIZE( m_serviceProvider, Menge::LoggerServiceInterface );

        SERVICE_PROVIDER_FINALIZE( m_serviceProvider );
    }
}
