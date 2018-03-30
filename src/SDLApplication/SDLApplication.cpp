#	include "Config/Config.h"

#	include "SDLApplication.h"

#	include "Engine/Application.h"

#	include "Interface/LoggerInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/PackageInterface.h"
#	include "Interface/UserdataInterface.h"
#   include "Interface/GraveyardInterface.h"
#   include "Interface/ResourceInterface.h"
#	include "Interface/TextInterface.h"
#	include "Interface/InputSystemInterface.h"
#	include "Interface/TimerInterface.h"
#	include "Interface/OptionsInterface.h"
#	include "Interface/HttpSystemInterface.h"

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
SERVICE_EXTERN( PrototypeService );
SERVICE_EXTERN( NodeService );
SERVICE_EXTERN( LoaderService );
SERVICE_EXTERN( RenderService );
SERVICE_EXTERN( RenderMaterialService );
SERVICE_EXTERN( RenderTextureService );
SERVICE_EXTERN( ResourceService );
SERVICE_EXTERN( TextService );
SERVICE_EXTERN( Watchdog );
SERVICE_EXTERN( ProfilerService );
SERVICE_EXTERN( Graveyard );
SERVICE_EXTERN( PackageService );
SERVICE_EXTERN( UserdataService );
SERVICE_EXTERN( PlayerService );
SERVICE_EXTERN( GameService );
SERVICE_EXTERN( TimelineService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_EXPORT( ImageCodec );
PLUGIN_EXPORT( SoundCodec );
PLUGIN_EXPORT( VideoCodec );
PLUGIN_EXPORT( Amplifier );
PLUGIN_EXPORT( Zip );
PLUGIN_EXPORT( LZ4 );
PLUGIN_EXPORT( BitmapFont );
#ifdef MENGINE_PLUGIN_TTF
#ifndef MENGINE_PLUGIN_TTF_DLL
PLUGIN_EXPORT( TTF )
#endif
#endif
#ifdef MENGINE_PLUGIN_SPINE
#ifndef MENGINE_PLUGIN_SPINE_DLL
PLUGIN_EXPORT( Spine );
#endif
#endif
PLUGIN_EXPORT( Movie );
PLUGIN_EXPORT( Box2D );
PLUGIN_EXPORT( OggVorbis );
PLUGIN_EXPORT( PathFinder );
PLUGIN_EXPORT( SDLFileGroup );
#ifdef MENGINE_PLUGIN_ASTRALAX
#ifndef MENGINE_PLUGIN_ASTRALAX_DLL
PLUGIN_EXPORT( AstralaxParticlePlugin2 );
#endif
#endif
#ifdef MENGINE_PLUGIN_STEAM
#ifndef MENGINE_PLUGIN_STEAM_DLL
PLUGIN_EXPORT( Steam );
#endif
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
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
        LOGGER_INFO("Inititalizing File Service...");
        SERVICE_CREATE( FileService );

        LOGGER_INFO("Initialize SDL file group...");
        PLUGIN_CREATE( SDLFileGroup );

        // mount root
        ConstString c_dir = Helper::stringizeString("dir");
        if( FILE_SERVICE()
            ->mountFileGroup( ConstString::none(), ConstString::none(), FilePath(ConstString::none()), c_dir ) == false )
        {
            LOGGER_ERROR("SDLApplication::setupFileService: failed to mount application directory"
                );

            return false;
        }

#	ifndef MENGINE_MASTER_RELEASE
        ConstString c_dev = Helper::stringizeString( "dev" );
        // mount root
        if( FILE_SERVICE()
            ->mountFileGroup( c_dev, ConstString::none(), FilePath(ConstString::none()), c_dir ) == false )
        {
            LOGGER_ERROR("SDLApplication::setupFileService: failed to mount dev directory"
                );

            return false;
        }
#   endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::getApplicationPath_( const char * _section, const char * _key, ConstString & _path )
    {
        FilePath applicationPath = STRINGIZE_FILEPATH_LOCAL( "application.ini" );

        IniUtil::IniStore ini;
        if( IniUtil::loadIni( ini, ConstString::none(), applicationPath ) == false )
        {
            LOGGER_ERROR("SDLApplication::initializeConfigEngine_ Invalid load application settings %s"
                , applicationPath.c_str()
                );

            return false;
        }

        const char * gameIniPath = ini.getSettingValue( _section, _key );

        if( gameIniPath == nullptr )
        {
            LOGGER_ERROR("SDLApplication::initializeConfigEngine_ Not found Game Path %s"
                , applicationPath.c_str()
                );

            return false;
        }

        _path = Helper::stringizeString( gameIniPath );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeConfigEngine_()
    {
        LOGGER_WARNING("Inititalizing Config Manager..." );
        SERVICE_CREATE( ConfigService );

        FilePath gameIniPath;
        if( this->getApplicationPath_( "Game", "Path", gameIniPath ) == false )
        {
            return false;
        }

        if( CONFIG_SERVICE()
            ->loadConfig( ConstString::none(), gameIniPath ) == false )
        {
            LOGGER_ERROR("SDLApplication::initializeConfigEngine_ invalid load config %s"
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
        size_t userPathLen = PLATFORM_SERVICE()
            ->getUserPath( userPathW, MENGINE_MAX_PATH );

        Char utf8_userPath[MENGINE_MAX_PATH];
        size_t utf8_userPathLen;
        UNICODE_SYSTEM()
            ->unicodeToUtf8( userPathW, userPathLen, utf8_userPath, MENGINE_MAX_PATH, &utf8_userPathLen );

        utf8_userPath[utf8_userPathLen] = '\0';

        FilePath cs_userPath = Helper::stringizeFilePath( utf8_userPath, utf8_userPathLen );
		
        // mount user directory
        if( FILE_SERVICE()
            ->mountFileGroup( Helper::stringizeString( "user" ), ConstString::none(), cs_userPath, Helper::stringizeString( "global" ) ) == false )
        {
            LOGGER_ERROR("SDLApplication: failed to mount user directory %s"
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
    bool SDLApplication::initializeLoggerEngine_()
    {
        SERVICE_CREATE( LoggerService );

        bool nologs = HAS_OPTION( "nologs" );

        if( nologs == true )
        {
            return true;
        }

		m_loggerStdio = new FactorableUnique<SDLStdioLogger>();

		m_loggerStdio->setVerboseFlag( LM_LOG );

		LOGGER_SERVICE()
			->registerLogger( m_loggerStdio );

        m_loggerMessageBox = new FactorableUnique<SDLMessageBoxLogger>();

        m_loggerMessageBox->setVerboseLevel( LM_CRITICAL );

        LOGGER_SERVICE()
            ->registerLogger( m_loggerMessageBox );

        EMessageLevel m_logLevel;

        bool developmentMode = HAS_OPTION( "dev" );

        if( developmentMode == true  )
        {
            m_logLevel = LM_LOG;
        }
        else
        {
            m_logLevel = LM_WARNING;
        }        

        const Char * option_log = GET_OPTION_VALUE( "log" );
        
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

        LOGGER_SERVICE()
            ->setVerboseLevel( m_logLevel );

        if( HAS_OPTION( "verbose" ) == true )
        {
            LOGGER_SERVICE()
                ->setVerboseLevel( LM_MAX );

            LOGGER_INFO("Verbose logging mode enabled");
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeArchiveService_()
    {
        LOGGER_INFO( "Inititalizing Archive Service..." );

        {
            LOGGER_INFO( "initialize Zip..." );
            
            PLUGIN_CREATE( Zip );
        }

        {
            LOGGER_INFO( "initialize LZ4..." );

            PLUGIN_CREATE( LZ4 );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeRenderEngine_()
    {
        LOGGER_INFO( "Initializing Render Service..." );

        if( SERVICE_CREATE( RenderSystem ) == false )
        {
            return false;
        }

        if( SERVICE_CREATE( RenderTextureService ) == false )
        {
            return false;
        }

        if( SERVICE_CREATE( RenderMaterialService ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLApplication::initializeSoundEngine_()
    {
        LOGGER_INFO( "Initializing Sound Service..." );

        SERVICE_CREATE( SoundSystem );

        bool muteMode = HAS_OPTION( "mute" );

        if( muteMode == true || SERVICE_EXIST( Mengine::SoundSystemInterface ) == false )
        {
            SERVICE_CREATE( SilentSoundSystem );
        }

        if( SERVICE_CREATE( SoundService ) == false )
        {
            LOGGER_ERROR("SDLApplication::initializeSoundEngine_ Failed to create Sound Engine"
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
        
        SERVICE_PROVIDER_SETUP( serviceProvider );

        m_serviceProvider = serviceProvider;

        SERVICE_CREATE( FactoryService );

        SERVICE_CREATE( OptionsService );

		TVectorString options;

		for( int i = 1; i < argc; ++i )
		{
			const char * arg = argv[i];

			options.push_back( arg );
		}

		OPTIONS_SERVICE()
			->setArgs( options );

        SERVICE_CREATE( StringizeService );

        if( this->initializeLoggerEngine_() == false )
        {
            return false;
        }

        SERVICE_CREATE( UnicodeSystem );
        
        SERVICE_CREATE( SDLLayer );
        SERVICE_CREATE( Platform );

        SERVICE_CREATE( PluginSystem );
        SERVICE_CREATE( PluginService );

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


        SERVICE_CREATE( ArchiveService );

        if( this->initializeArchiveService_() == false )
        {
            return false;
        }

        SERVICE_CREATE( ThreadSystem );
        SERVICE_CREATE( ThreadService );

        SERVICE_CREATE( NotificationService );

        if( this->initializeRenderEngine_() == false )
        {
            return false;
        }

        if( this->initializeSoundEngine_() == false )
        {
            return false;
        }
        
        SERVICE_CREATE( ScriptService );
        SERVICE_CREATE( ModuleService );
		SERVICE_CREATE( CodecService );
        SERVICE_CREATE( DataService );
		SERVICE_CREATE( PrefetcherService );
        SERVICE_CREATE( MemoryService );
		SERVICE_CREATE( ConverterService );
        SERVICE_CREATE( InputService );

		SERVICE_CREATE( HttpSystem );

        SERVICE_CREATE( TimerSystem );
        SERVICE_CREATE( TimerService );

        PythonScriptWrapper::constsWrap();
        PythonScriptWrapper::mathWrap();
        PythonScriptWrapper::nodeWrap();
        PythonScriptWrapper::helperWrap();
        PythonScriptWrapper::soundWrap();
        PythonScriptWrapper::entityWrap();

        SERVICE_CREATE( PrototypeService );
        SERVICE_CREATE( NodeService );
        SERVICE_CREATE( LoaderService );

        SERVICE_CREATE( RenderService );
        SERVICE_CREATE( RenderMaterialService );
        SERVICE_CREATE( RenderTextureService );

        SERVICE_CREATE( ResourceService );
        SERVICE_CREATE( TextService );
        SERVICE_CREATE( Watchdog );
        SERVICE_CREATE( ProfilerService );
        SERVICE_CREATE( Graveyard );
        SERVICE_CREATE( PackageService );
        SERVICE_CREATE( UserdataService );
        SERVICE_CREATE( PlayerService );
        SERVICE_CREATE( GameService );
        SERVICE_CREATE( TimelineService );
        
        SERVICE_CREATE( Application );

#   define MENGINE_ADD_PLUGIN( Name, Info )\
        do{LOGGER_INFO( Info );\
        if(	PLUGIN_CREATE(Name) == false ){\
        LOGGER_ERROR( "Invalid %s", Info );}else{\
        LOGGER_WARNING( "Successful %s", Info );}}while(false, false)

        MENGINE_ADD_PLUGIN(ImageCodec, "initialize Plugin Image Codec...");
        MENGINE_ADD_PLUGIN(SoundCodec, "initialize Plugin Sound Codec...");
        MENGINE_ADD_PLUGIN(OggVorbis, "initialize Plugin Ogg Vorbis Codec...");
        MENGINE_ADD_PLUGIN(Amplifier, "initialize Plugin Amplifier...");
        MENGINE_ADD_PLUGIN(VideoCodec, "initialize Plugin Video Codec...");

		MENGINE_ADD_PLUGIN( BitmapFont, "initialize Plugin TTF..." );

#ifdef MENGINE_PLUGIN_TTF
#ifndef MENGINE_PLUGIN_TTF_DLL
		MENGINE_ADD_PLUGIN( TTF, "initialize Plugin TTF..." );
#endif
#endif

#ifdef MENGINE_PLUGIN_SPINE
#ifndef MENGINE_PLUGIN_SPINE_DLL
        MENGINE_ADD_PLUGIN(Spine, "initialize Plugin Spine...");
#endif
#endif
        MENGINE_ADD_PLUGIN(Movie, "initialize Plugin Movie...");
        //MENGINE_ADD_PLUGIN(Motor, "initialize Plugin Motor...");
        MENGINE_ADD_PLUGIN(Box2D, "initialize Plugin Box2D...");

        MENGINE_ADD_PLUGIN(PathFinder, "initialize Plugin Path Finder...");

#ifdef MENGINE_PLUGIN_ASTRALAX
#ifndef MENGINE_PLUGIN_ASTRALAX_DLL
		MENGINE_ADD_PLUGIN( AstralaxParticlePlugin2, "initialize Astralax Particle Plugin..." );
#endif
#endif


#ifdef MENGINE_PLUGIN_STEAM
#ifndef MENGINE_PLUGIN_STEAM_DLL
        MENGINE_ADD_PLUGIN( Steam, "initialize Steam Plugin..." );
#endif
#endif

#   undef MENGINE_ADD_PLUGIN

        TVectorWString plugins;
        CONFIG_VALUES("Plugins", "Name", plugins);

        for( TVectorWString::const_iterator
            it = plugins.begin(),
            it_end = plugins.end();
        it != it_end;
        ++it )
        {
            const WString & pluginName = *it;

            if( PLUGIN_SERVICE()
                ->loadPlugin( pluginName ) == false )
            {
                LOGGER_ERROR("Application Failed to load plugin %ls"
                    , pluginName.c_str()
                    );

                return false;
            }
        }

#	ifdef _DEBUG
		bool devplugins = true;
#	else
		bool developmentMode = HAS_OPTION( "dev" );
		bool devplugins = developmentMode;
#	endif

		bool nodevplugins = HAS_OPTION( "nodevplugins" );

        if( devplugins == true && nodevplugins == false )
        {
            TVectorWString devPlugins;
            CONFIG_VALUES( "DevPlugins", "Name", devPlugins );

            for( TVectorWString::const_iterator
                it = devPlugins.begin(),
                it_end = devPlugins.end();
                it != it_end;
                ++it )
            {
                const WString & pluginName = *it;

                if( PLUGIN_SERVICE()
                    ->loadPlugin( pluginName ) == false )
                {
                    LOGGER_WARNING("Application Failed to load dev plugin %ls"
                        , pluginName.c_str()
                        );
                }
            }
        }

        SERVICE_CREATE(ParticleService);

        TVectorString modules;
        CONFIG_VALUES("Modules", "Name", modules);

        for( TVectorString::const_iterator
            it = modules.begin(),
            it_end = modules.end();
        it != it_end;
        ++it )
        {
            const String & moduleName = *it;

            if( MODULE_SERVICE()
                ->runModule( Helper::stringizeString(moduleName) ) == false )
            {
                LOGGER_ERROR("Application Failed to run module %s"
                    , moduleName.c_str()
                    );

                return false;
            }
        }

		FilePath renderMaterialsPathEmpty;
        FilePath renderMaterialsPath = CONFIG_VALUE( "Engine", "RenderMaterials", renderMaterialsPathEmpty );

        if( renderMaterialsPath.empty() == false )
        {
            if( RENDERMATERIAL_SERVICE()
                ->loadMaterials( ConstString::none(), renderMaterialsPath ) == false )
            {
                return false;
            }
        }

        LOGGER_INFO("Application Create...");

        FilePath resourceIniPath;
        if( this->getApplicationPath_( "Resource", "Path", resourceIniPath ) == false )
        {
            LOGGER_CRITICAL("Application invalid setup resource path"
                );

            return false;
        }

        if( APPLICATION_SERVICE()
            ->initializeGame( ConstString::none(), resourceIniPath ) == false )
        {
            LOGGER_CRITICAL("Application invalid initialize game"
                );

            return false;
        }
               
        PLATFORM_SERVICE()
            ->setIcon( 0 );

        const String & projectTitle = APPLICATION_SERVICE()
            ->getProjectTitle();

        WString wprojectTitle;
        if( Helper::utf8ToUnicode(projectTitle, wprojectTitle) == false )
        {
            LOGGER_ERROR("Application project title %s not convert to unicode"
                                            , projectTitle.c_str()
                                            );
        }

        PLATFORM_SERVICE()
            ->setProjectTitle( wprojectTitle );

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
            ->update();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLApplication::finalize()
    {
        SERVICE_FINALIZE( Mengine::ModuleServiceInterface );

        PLATFORM_SERVICE()
            ->stopPlatform();

        NOTIFICATION_SERVICE()
            ->notify( NOTIFICATOR_ENGINE_FINALIZE );

        SERVICE_PROVIDER_STOP();

        THREAD_SERVICE()
            ->stopTasks();

        SERVICE_FINALIZE( Mengine::GameServiceInterface );
        SERVICE_FINALIZE( Mengine::PlayerServiceInterface );
        SERVICE_FINALIZE( Mengine::PackageServiceInterface );
        SERVICE_FINALIZE( Mengine::UserdataServiceInterface );
        SERVICE_FINALIZE( Mengine::GraveyardInterface );
        SERVICE_FINALIZE( Mengine::NodeServiceInterface );
        SERVICE_FINALIZE( Mengine::ResourceServiceInterface );
        SERVICE_FINALIZE( Mengine::TextServiceInterface );
        SERVICE_FINALIZE( Mengine::PrototypeServiceInterface );
        SERVICE_FINALIZE( Mengine::ApplicationInterface );
        SERVICE_FINALIZE( Mengine::HttpSystemInterface );
        SERVICE_FINALIZE( Mengine::PrefetcherServiceInterface );
        SERVICE_FINALIZE( Mengine::DataServiceInterface );
        SERVICE_FINALIZE( Mengine::PluginServiceInterface );
        SERVICE_FINALIZE( Mengine::InputServiceInterface );
        SERVICE_FINALIZE( Mengine::UnicodeSystemInterface );

        SERVICE_FINALIZE( Mengine::CodecServiceInterface );
        SERVICE_FINALIZE( Mengine::ParticleSystemInterface2 );
        SERVICE_FINALIZE( Mengine::ParticleServiceInterface2 );

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
        SERVICE_FINALIZE( Mengine::ThreadSystemInterface );

        SERVICE_FINALIZE( Mengine::TimerServiceInterface );
        SERVICE_FINALIZE( Mengine::TimerSystemInterface );

        SERVICE_FINALIZE( Mengine::PlatformInterface );
        
        if( m_fileLog != nullptr )
        {
            LOGGER_SERVICE()
                ->unregisterLogger( m_fileLog );

            m_fileLog = nullptr;
        }

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

        SERVICE_FINALIZE( Mengine::FileServiceInterface );
        SERVICE_FINALIZE( Mengine::LoggerServiceInterface );

        SERVICE_PROVIDER_FINALIZE( m_serviceProvider );
    }
}
