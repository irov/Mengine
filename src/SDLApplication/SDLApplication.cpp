#	include "SDLApplication.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/InputSystemInterface.h"

#	include <cstdio>
#	include <clocale>

#	include "Core/FileLogger.h"
#	include "Core/IniUtil.h"

#	include "Logger/Logger.h"

#	include <ctime>
#	include <algorithm>

#	include <sstream>
#	include <iomanip>

#	include "SDL.h"

#	ifdef _WIN32
#	pragma comment(lib, "Winmm.lib")
#	pragma comment(lib, "Version.lib")
#	pragma comment(lib, "Imm32.lib")
#	endif

SERVICE_EXTERN(ServiceProvider, Menge::ServiceProviderInterface);
SERVICE_EXTERN(Application, Menge::ApplicationInterface);
SERVICE_EXTERN(StringizeService, Menge::StringizeServiceInterface);
SERVICE_EXTERN(LogService, Menge::LogServiceInterface);
SERVICE_EXTERN(SDLLayer, Menge::SDLLayerInterface);

SERVICE_EXTERN(ArchiveService, Menge::ArchiveServiceInterface);

SERVICE_EXTERN(ThreadSystem, Menge::ThreadSystemInterface);
SERVICE_EXTERN(ThreadService, Menge::ThreadServiceInterface);

//SERVICE_EXTERN(ParticleSystem, Menge::ParticleSystemInterface);
SERVICE_EXTERN(ParticleService, Menge::ParticleServiceInterface);

SERVICE_EXTERN(ParticleSystem2, Menge::ParticleSystemInterface2);
SERVICE_EXTERN(ParticleService2, Menge::ParticleServiceInterface2);

SERVICE_EXTERN(RenderSystem, Menge::RenderSystemInterface);
SERVICE_EXTERN(RenderService, Menge::RenderServiceInterface);
SERVICE_EXTERN(RenderTextureManager, Menge::RenderTextureServiceInterface);
SERVICE_EXTERN(RenderMaterialManager, Menge::RenderMaterialServiceInterface);

SERVICE_EXTERN(PhysicSystem, Menge::PhysicSystemInterface);

SERVICE_EXTERN(UnicodeSystem, Menge::UnicodeSystemInterface);
SERVICE_EXTERN(UnicodeService, Menge::UnicodeServiceInterface);

SERVICE_EXTERN(FileService, Menge::FileServiceInterface);

SERVICE_EXTERN(NotificationService, Menge::NotificationServiceInterface);
SERVICE_EXTERN(ScriptService, Menge::ScriptServiceInterface);

SERVICE_EXTERN(SoundSystem, Menge::SoundSystemInterface);
SERVICE_EXTERN(SilentSoundSystem, Menge::SoundSystemInterface);
SERVICE_EXTERN(SoundService, Menge::SoundServiceInterface);

SERVICE_EXTERN(InputService, Menge::InputServiceInterface);
SERVICE_EXTERN(CodecService, Menge::CodecServiceInterface);
SERVICE_EXTERN(PluginService, Menge::PluginServiceInterface);

SERVICE_EXTERN(ModuleService, Menge::ModuleServiceInterface);
SERVICE_EXTERN(DataService, Menge::DataServiceInterface);
SERVICE_EXTERN(CacheService, Menge::CacheServiceInterface);
SERVICE_EXTERN(ConfigService, Menge::ConfigServiceInterface);
SERVICE_EXTERN(PrefetcherService, Menge::PrefetcherServiceInterface);


PLUGIN_EXPORT( MengeImageCodec );
PLUGIN_EXPORT( MengeSoundCodec );
PLUGIN_EXPORT( MengeVideoCodec );
PLUGIN_EXPORT( MengeAmplifier );
PLUGIN_EXPORT( MengeZip );
PLUGIN_EXPORT( MengeLZ4 );

PLUGIN_EXPORT( MengeOggVorbis );

PLUGIN_EXPORT( PathFinder );

PLUGIN_EXPORT( MengePosixFileGroup );

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    static const float s_activeFrameTime = 1000.f/60.f;
    static const float s_inactiveFrameTime = 100;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        static void s_getOption( const Menge::String & _option
            , const Menge::String & _commandLine
            , Menge::String* _value )
        {
            if( _value == NULL )
            {
                return;
            }

            Menge::String::size_type option_index = 0;
            while( (option_index = _commandLine.find( _option, option_index )) != Menge::String::npos )
            {
                option_index += _option.length();
                if( option_index >= _commandLine.length() )
                {
                    break;
                }
                char next_delim = _commandLine[option_index] == '\"' ? '\"' : ' ';
                Menge::String::size_type next_index = _commandLine.find( next_delim, option_index+1 );
                if( next_delim == '\"' )
                {
                    ++option_index;
                }

                Menge::String::size_type value_length = next_index - option_index;
                (*_value) += _commandLine.substr( option_index, value_length );
                _value->push_back( ' ' );
            }

            if( _value->empty() == false )
            {
                _value->erase( _value->length() - 1 );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_hasOption( const char * _option, const Menge::String& _commandLine )
        {
            if( _commandLine.find( _option ) == Menge::String::npos )
            {
                return false;
            }

            return true;
        }
    }
    //////////////////////////////////////////////////////////////////////////
	SDLApplication::SDLApplication()
        : m_serviceProvider(nullptr)
        , m_application(nullptr)
        , m_loggerConsole(nullptr)
        , m_fileLog(nullptr)
        , m_timer(nullptr)
		, m_input( nullptr )
		, m_running(true)
		, m_active(false)
		, m_inputService(nullptr)
		, m_unicodeSystem(nullptr)
		, m_unicodeService(nullptr)
		, m_logService(nullptr)
		, m_fileService(nullptr)
		, m_codecService(nullptr)
		, m_threadSystem(nullptr)
		, m_threadService(nullptr)
		, m_particleService(nullptr)
		, m_particleService2(nullptr)
		, m_renderSystem(nullptr)
		, m_renderService(nullptr)
		, m_renderTextureManager(nullptr)
		, m_renderMaterialManager(nullptr)
		, m_soundSystem(nullptr)
		, m_soundService(nullptr)
		, m_scriptService(nullptr)
		, m_archiveService(nullptr)
		, m_moduleService(nullptr)
		, m_dataService(nullptr)
		, m_cacheService(nullptr)
		, m_configService(nullptr)
		, m_prefetcherService(nullptr)
		, m_notificationService(nullptr)
		, m_stringizeService(nullptr)
		, m_developmentMode(false)
    {
    }
    //////////////////////////////////////////////////////////////////////////
	SDLApplication::~SDLApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
	const String & SDLApplication::getCurrentPath() const
    {
        static String empty;
        return empty;
    }
    //////////////////////////////////////////////////////////////////////////
	size_t SDLApplication::getShortPathName( const String & _path, char * _shortpath, size_t _shortpathlen )
    {
		(void)_path;
		(void)_shortpath;
		(void)_shortpathlen;

        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
	TimerInterface * SDLApplication::getTimer() const
    {
        return m_timer;
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeApplicationService_()
    {
        ApplicationInterface * application;
        if( SERVICE_CREATE( Application, &application ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, application ) == false )
        {
            return false;
        }

        m_application = application;

        LOGGER_INFO(m_serviceProvider)( "Application Initialize..."
            );

        if( m_application->initialize( m_commandLine ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Application initialize failed" 
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeNotificationService_()
    {
        LOGGER_INFO(m_serviceProvider)( "Inititalizing Notification Service..." );

        NotificationServiceInterface * notificationService;
        if( SERVICE_CREATE( NotificationService, &notificationService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, notificationService ) == false )
        {
            return false;
        }

		m_notificationService = notificationService;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeThreadEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Inititalizing Thread Service..." );

        ThreadSystemInterface * threadSystem;
        if( SERVICE_CREATE( ThreadSystem, &threadSystem ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeThreadEngine_ failed to create ThreadSystem"
                );

            return false;
        }

        if( SERVICE_REGISTRY(m_serviceProvider, threadSystem) == false )
        {
            return false;
        }

        if( threadSystem->initialize() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeThreadEngine_ invalid initialize"
				);

			return false;
		}

		m_threadSystem = threadSystem;

        ThreadServiceInterface * threadService;
        if( SERVICE_CREATE( ThreadService, &threadService ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeThreadEngine_ failed to create ThreadService"
                );

            return false;               
        }

        if( SERVICE_REGISTRY( m_serviceProvider, threadService ) == false )
        {
            return false;
        }

        m_threadService = threadService;

        if( m_threadService->initialize( 2 ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeFileEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Inititalizing File Service..." );
		       
        FileServiceInterface * fileService;
        if( SERVICE_CREATE( FileService, &fileService ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initialize failed to create FileService"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, fileService ) == false )
        {
            return false;
        }

        m_fileService = fileService;

        if( m_fileService->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "WinApplication::setupFileService: failed to initialize fileService"
                );

            return false;
        }
				
		LOGGER_INFO( m_serviceProvider )("Initialize Win32 file group...");
		PluginInterface * plugin;		
		initPluginMengePosixFileGroup( &plugin );
		plugin->initialize( m_serviceProvider );
		m_plugins.push_back( plugin );

        // mount root		
        ConstString c_dir = Helper::stringizeString(m_serviceProvider, "dir");
        if( m_fileService->mountFileGroup( ConstString::none(), m_currentPath, c_dir ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "WinApplication::setupFileService: failed to mount application directory %ls"
                , m_currentPath.c_str()
                );

            return false;
        }

        //m_tempPath.clear();
        m_userPath.clear();

        m_userPath += L"User";
        m_userPath += MENGE_FOLDER_DELIM;

        String utf8_userPath;
        if( Helper::unicodeToUtf8( m_serviceProvider, m_userPath, utf8_userPath ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "WinApplication: failed user directory %ls convert to ut8f"
                , m_userPath.c_str()
                );

            return false;
        }

        FilePath userPath = Helper::stringizeString( m_serviceProvider, utf8_userPath );

        // mount user directory
        if( m_fileService->mountFileGroup( Helper::stringizeString(m_serviceProvider, "user"), userPath, Helper::stringizeString(m_serviceProvider, "dir") ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "WinApplication: failed to mount user directory %ls"
                , m_userPath.c_str()
                );

            return false;
        }

		if( m_fileService->existDirectory( Helper::stringizeString(m_serviceProvider, "user"), ConstString::none() ) == false )
		{
			m_fileService->createDirectory( Helper::stringizeString(m_serviceProvider, "user"), ConstString::none() );
		}

        FilePath logFilename = Helper::stringizeString( m_serviceProvider, "log.log" );

        OutputStreamInterfacePtr fileLogInterface = 
            m_fileService->openOutputFile( Helper::stringizeString(m_serviceProvider, "user"), logFilename );

        if( fileLogInterface != nullptr )
        {
            m_fileLog = new FileLogger(fileLogInterface);

            m_logService->registerLogger( m_fileLog );
        }

#	ifndef MENGINE_MASTER_RELEASE
		ConstString c_dev = Helper::stringizeString( m_serviceProvider, "dev" );
		// mount root		
		if( m_fileService->mountFileGroup( c_dev, ConstString::none(), c_dir ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "WinApplication::setupFileService: failed to mount dev directory %ls"
				, m_currentPath.c_str()
				);

			return false;
		}
#   endif

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool SDLApplication::getApplicationPath_( const char * _section, const char * _key, ConstString & _path )
	{
		FilePath applicationPath = STRINGIZE_STRING_LOCAL( m_serviceProvider, "application.ini" );

		InputStreamInterfacePtr applicationInputStream = 
			FILE_SERVICE(m_serviceProvider)->openInputFile( ConstString::none(), applicationPath, false );

		if( applicationInputStream == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeConfigEngine_ Invalid open application settings %s"
				, applicationPath.c_str()
				);

			return false;
		}

		IniUtil::IniStore ini;
		if( IniUtil::loadIni( ini, applicationInputStream, m_serviceProvider ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeConfigEngine_ Invalid load application settings %s"
				, applicationPath.c_str()
				);

			return false;
		}

		const char * gameIniPath = ini.getSettingValue( _section, _key );

		if( gameIniPath == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeConfigEngine_ Not found Game Path %s"
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

		ConfigServiceInterface * configService;

		if( SERVICE_CREATE(ConfigService, &configService) == false )
		{
			return false;
		}

		SERVICE_REGISTRY(m_serviceProvider, configService);

		m_configService = configService;

		if( m_configService->initialize( m_platformName ) == false )
		{
			return false;
		}

		FilePath gameIniPath;
		if( this->getApplicationPath_( "Game", "Path", gameIniPath ) == false )
		{
			return false;
		}

		if( CONFIG_SERVICE(m_serviceProvider)
			->loadConfig( ConstString::none(), gameIniPath ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeConfigEngine_ invalid load config %s"				
				, gameIniPath.c_str()
				);

			return false;
		}

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeStringizeService_()
    {
        StringizeServiceInterface * stringizeService;
        if( SERVICE_CREATE( StringizeService, &stringizeService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, stringizeService ) == false )
        {
            return false;
        }

		m_stringizeService = stringizeService;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeLogEngine_()
    {
        LogServiceInterface * logService;
        if( SERVICE_CREATE( LogService, &logService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, logService ) == false )
        {
            return false;
        }

        m_logService = logService;

        m_loggerConsole = new SDLLogger();

        m_logService->registerLogger( m_loggerConsole );

        EMessageLevel m_logLevel;

        m_logLevel = LM_LOG;

        String logLevel;
        Helper::s_getOption( " -log:", m_commandLine, &logLevel );

        if( logLevel == "0" )
        {
            m_logLevel = LM_INFO;
        }
        else if ( logLevel == "1" )
        {
            m_logLevel = LM_LOG;
        }
        else if ( logLevel == "2" )
        {
            m_logLevel = LM_WARNING;
        }
        else if ( logLevel == "3" )
        {
            m_logLevel = LM_ERROR;
        }

        m_logService->setVerboseLevel( m_logLevel );

        if( Helper::s_hasOption( " -verbose ", m_commandLine ) == true )
        {
            m_logService->setVerboseLevel( LM_MAX );

            LOGGER_INFO(m_serviceProvider)( "Verbose logging mode enabled" );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeSDLLayerService_()
    {
        LOGGER_INFO(m_serviceProvider)( "Inititalizing SDLLayer Service..." );

        SDLLayerInterface * sdlLayer;
		if( SERVICE_CREATE( SDLLayer, &sdlLayer ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeMarmaladeLayerService_ failed to create MarmaladeLayer"
                );

            return false;
        }

		if( SERVICE_REGISTRY( m_serviceProvider, sdlLayer ) == false )
        {
            return false;
        }

		m_layer = sdlLayer;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeArchiveService_()
    {
        LOGGER_INFO(m_serviceProvider)( "Inititalizing Archive Service..." );

        ArchiveServiceInterface * archiveService;
        if( SERVICE_CREATE( ArchiveService, &archiveService ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeArchiveService_ failed to create ArchiveService"
                );

            return false;
        }

        if( SERVICE_REGISTRY(m_serviceProvider, archiveService) == false )
        {
            return false;
        }

        m_archiveService = archiveService;

		{
			LOGGER_INFO(m_serviceProvider)( "initialize Zip..." );

			PluginInterface * pluginMengeZip;
			initPluginMengeZip( &pluginMengeZip );
			pluginMengeZip->initialize( m_serviceProvider );
			m_plugins.push_back( pluginMengeZip );
		}

		{
			LOGGER_INFO(m_serviceProvider)( "initialize LZ4..." );

			PluginInterface * pluginMengeLZ4;
			initPluginMengeLZ4( &pluginMengeLZ4 );
			pluginMengeLZ4->initialize( m_serviceProvider );
			m_plugins.push_back( pluginMengeLZ4 );
		}

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeUnicodeEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Unicode Service..." );

        UnicodeSystemInterface * unicodeSystem;
        if( SERVICE_CREATE( UnicodeSystem, &unicodeSystem ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, unicodeSystem ) == false )
        {
            return false;
        }

		m_unicodeSystem = unicodeSystem;

        UnicodeServiceInterface * unicodeService;
        if( SERVICE_CREATE( UnicodeService, &unicodeService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, unicodeService ) == false )
        {
            return false;
        }

        m_unicodeService = unicodeService;

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeParticleEngine2_()
	{
		LOGGER_INFO(m_serviceProvider)( "Initializing Particle Service 2..." );

		ParticleSystemInterface2 * particleSystem;
		if( SERVICE_CREATE( ParticleSystem2, &particleSystem ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeApplication::initializeParticleEngine2_ Failed to create ParticleSystem2"
				);

			return false;
		}

		if( particleSystem->initialize() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeApplication::initializeParticleEngine2_ Failed to initialize ParticleSystem2"
				);

			return false;
		}

		if( SERVICE_REGISTRY( m_serviceProvider, particleSystem ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("MarmaladeApplication::initializeParticleEngine2_ Failed to registry ParticleSystem2"
				);

			return false;
		}

		ParticleServiceInterface2 * particleService;
		if( SERVICE_CREATE( ParticleService2, &particleService ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializeParticleEngine2_ Failed to create ParticleService2"
				);

			return false;
		}

		if( SERVICE_REGISTRY( m_serviceProvider, particleService ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializeParticleEngine2_ Failed to create ParticleService2"
				);

			return false;
		}

		if( particleService->initialize() == false )
		{
			SERVICE_UNREGISTRY( m_serviceProvider, particleService );
			SERVICE_UNREGISTRY( m_serviceProvider, particleSystem );

			LOGGER_ERROR( m_serviceProvider )("MarmaladeApplication::initializeParticleEngine2_ Failed to initialize ParticleService2"
				);

			return false;
		}

		m_particleService2 = particleService;

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializePhysicEngine2D_()
    {
        //LOGGER_INFO(m_serviceProvider)( "Inititalizing Physics2D Service..." );

        //PhysicSystem2DInterface * physicSystem2D;
        //if( createPhysicSystem2D( &physicSystem2D ) == false )
        //{
        //    LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializePhysicEngine2D_ Failed to initialize PhysicSystem2D"
        //        );

        //    return false;
        //}

        //if( SERVICE_REGISTRY( m_serviceProvider, physicSystem2D ) == false )
        //{
        //    return false;
        //}

        //if( physicSystem2D->initialize() == false )
        //{
        //    LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializePhysicEngine2D_ Failed to initialize Physics System 2D"
        //        );

        //    return false;
        //}

        //PhysicService2DInterface * physicService2D;
        //if( createPhysicService2D( &physicService2D ) == false )
        //{
        //    LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializePhysicEngine2D_ Failed to create Physic Service 2D"
        //        );

        //    return false;
        //}

        //if( SERVICE_REGISTRY( m_serviceProvider, physicService2D ) == false )
        //{
        //    return false;
        //}

        //m_physicService2D = physicService2D;

        //if( m_physicService2D->initialize() == false )
        //{
        //    LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializePhysicEngine2D_ Failed to initialize Physics Service 2D"
        //        );

        //    return false;
        //}

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeRenderEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Render Service..." );

        RenderSystemInterface * renderSystem;
        if( SERVICE_CREATE( RenderSystem, &renderSystem ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, renderSystem ) == false )
        {
            return false;
        }

        if( renderSystem->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializeRenderEngine_ Failed to initialize Render System"
                );

            return false;
        }

		m_renderSystem = renderSystem;

        RenderServiceInterface * renderService;
        if( SERVICE_CREATE( RenderService, &renderService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, renderService ) == false )
        {
            return false;
        }

        m_renderService = renderService;

        if( m_renderService->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializeRenderEngine_ Failed to initialize Render Engine"
                );

            return false;
        }

		RenderTextureServiceInterface * renderTextureManager;
		if( SERVICE_CREATE( RenderTextureManager, &renderTextureManager) == false )
		{
			return false;
		}

		if( SERVICE_REGISTRY( m_serviceProvider, renderTextureManager ) == false )
		{
			return false;
		}

		m_renderTextureManager = renderTextureManager;

		if( m_renderTextureManager->initialize() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeRenderEngine_ Failed to initialize Render Texture Service"
				);

			return false;
		}

		RenderMaterialServiceInterface * renderMaterialManager;
		if( SERVICE_CREATE( RenderMaterialManager, &renderMaterialManager ) == false )
		{
			return false;
		}

		if( SERVICE_REGISTRY( m_serviceProvider, renderMaterialManager ) == false )
		{
			return false;
		}

		m_renderMaterialManager = renderMaterialManager;

		if( m_renderMaterialManager->initialize() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeRenderEngine_ Failed to initialize Render Material Service"
				);

			return false;
		}

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeSoundEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Sound Service..." );

        SoundSystemInterface * soundSystem;
        if( SERVICE_CREATE( SoundSystem, &soundSystem ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY(m_serviceProvider, soundSystem) == false )
        {
            return false;
        }

        if( soundSystem->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializeSoundEngine_ Failed to initialize Sound System"
                );

            return false;
        }

		m_soundSystem = soundSystem;

        SoundServiceInterface * soundService;
        if( SERVICE_CREATE( SoundService, &soundService ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializeSoundEngine_ Failed to create Sound Engine"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, soundService ) == false )
        {
            return false;
        }

        m_soundService = soundService;

        if( m_soundService->initialize( false, true ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializeSoundEngine_ Failed to initialize Sound Engine"
                );

            m_serviceProvider->unregistryService( "SoundService" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeSilentSoundEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Silent Sound Service..." );

        SoundSystemInterface * soundSystem;
        if( SERVICE_CREATE( SilentSoundSystem, &soundSystem ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY(m_serviceProvider, soundSystem) == false )
        {
            return false;
        }

        if( soundSystem->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializeSoundEngine_ Failed to initialize Silent Sound System"
                );

            return false;
        }

        SoundServiceInterface * soundService;
        if( SERVICE_CREATE( SoundService, &soundService ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("Application::initializeSilentSoundEngine_ Failed to create Sound Engine"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, soundService ) == false )
        {
            return false;
        }

        m_soundService = soundService;

        if( m_soundService->initialize( true, false ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializeSilentSoundEngine_ Failed to initialize Sound Engine"
                );

            return false;                
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeScriptEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Script Service..." );

        ScriptServiceInterface * scriptService;
        if( SERVICE_CREATE( ScriptService, &scriptService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, scriptService ) == false )
        {
            return false;
        }

        m_scriptService = scriptService;

        if( m_scriptService->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializeScriptEngine_ Failed to initialize Script Engine"
                );

            return false;
        }

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeModuleEngine_()
	{
		LOGGER_INFO(m_serviceProvider)( "Initializing Module Service..." );

		ModuleServiceInterface * moduleService;
		if( SERVICE_CREATE( ModuleService, &moduleService ) == false )
		{
			return false;
		}

		if( SERVICE_REGISTRY( m_serviceProvider, moduleService ) == false )
		{
			return false;
		}

		m_moduleService = moduleService;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeDataManager_()
	{
		LOGGER_INFO(m_serviceProvider)( "Inititalizing Data Manager..." );

		DataServiceInterface * dataService;

		if( SERVICE_CREATE( DataService, &dataService ) == false )
		{
			return false;
		}

		SERVICE_REGISTRY( m_serviceProvider, dataService );

		if( dataService->initialize() == false )
		{
			return false;
		}

		m_dataService = dataService;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeCacheManager_()
	{
		LOGGER_INFO(m_serviceProvider)( "Inititalizing Cache Manager..." );

		CacheServiceInterface * cacheService;

		if( SERVICE_CREATE( CacheService, &cacheService ) == false )
		{
			return false;
		}

		SERVICE_REGISTRY( m_serviceProvider, cacheService );

		if( cacheService->initialize() == false )
		{
			return false;
		}

		m_cacheService = cacheService;

		return true;
	}	
	//////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializePrefetcherService_()
	{
		LOGGER_INFO(m_serviceProvider)( "Inititalizing Prefetcher Service..." );

		PrefetcherServiceInterface * prefetcherService;

		if( SERVICE_CREATE( PrefetcherService, &prefetcherService ) == false )
		{
			return false;
		}

		SERVICE_REGISTRY( m_serviceProvider, prefetcherService );

		if( prefetcherService->initialize() == false )
		{
			return false;
		}

		m_prefetcherService = prefetcherService;

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeCodecEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Codec Service..." );

        CodecServiceInterface * codecService;
        if( SERVICE_CREATE( CodecService, &codecService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, codecService ) == false )
        {
            return false;
        }

        m_codecService = codecService;
		
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeInputEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Input Service..." );

        InputServiceInterface * inputService;
        if( SERVICE_CREATE( InputService, &inputService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, inputService ) == false )
        {
            return false;
        }

        //bool result = m_inputEngine->initialize( m_serviceProvider );
        //if( result == false )
        //{
        //    MENGE_LOG_ERROR( "Input Engine initialization failed!" );
        //    return false;
        //}

        m_inputService = inputService;

        if( m_inputService->initialize() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeInputEngine_ Failed to initialize Input Engine"
                );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializePluginService_()
    {
        //LOGGER_INFO(m_serviceProvider)( "Initializing Plugin Service..." );

        //PluginServiceInterface * pluginService;
        //if( createPluginService( &pluginService ) == false )
        //{
        //    return false;
        //}

        //if( SERVICE_REGISTRY(m_serviceProvider, pluginService) == false )
        //{
        //    return false;
        //}

        //m_pluginService = pluginService;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initializeNodeManager_()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLApplication::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        (void)_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * SDLApplication::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLApplication::getMaxClientResolution( Resolution & _resolution ) const
    {
		int w;
		int h;
		SDL_GetWindowSize( m_window, &w, &h );

		float width = static_cast<float>(w);
		float height = static_cast<float>(h);

		_resolution.setWidth( width );
		_resolution.setHeight( height );
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::initialize( int argc, char * argv[] )
    {
		for( int i = 1; i != argc; ++i )
		{
			m_commandLine += " ";
			m_commandLine += argv[i];
			m_commandLine += " ";
		}		

        setlocale( LC_CTYPE, "" );

        String scriptInit;
        Helper::s_getOption( " -s:", m_commandLine, &scriptInit );

		if( Helper::s_hasOption( " -dev ", m_commandLine ) == true )
		{
			m_developmentMode = true;
		}

		SDL_SetHint( SDL_HINT_VIDEO_ALLOW_SCREENSAVER, "1" );

		SDL_Init( SDL_INIT_VIDEO );
		
		SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 6 );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
		SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 0 );
		SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 0 );
		SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
		SDL_GL_SetAttribute( SDL_GL_RETAINED_BACKING, 0 );
		SDL_GL_SetAttribute( SDL_GL_ACCELERATED_VISUAL, 1 );
		
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );
		SDL_GL_SetAttribute( SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1 );

		//SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 4 );
		//SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );

		{
			int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

			SDL_DisplayMode mode;
			SDL_GetCurrentDisplayMode( 0, &mode );

			m_window = SDL_CreateWindow( "Mengine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, flags );

			m_context = SDL_GL_CreateContext( m_window );
		}

        ServiceProviderInterface * serviceProvider;
        if( SERVICE_CREATE( ServiceProvider, &serviceProvider ) == false )
        {
            return false;
        }

        m_serviceProvider = serviceProvider;

        if( m_serviceProvider->registryService( "PlatformService", this ) == false )
        {
            return false;
        }

        if( this->initializeStringizeService_() == false )
        {
            return false;
        }
				
        if( this->initializeLogEngine_() == false )
        {
            return false;
        }

        if( this->initializeSDLLayerService_() == false )
        {
            return false;
        }

        if( this->initializeNotificationService_() == false )
        {
            return false;
        }

        if( this->initializeUnicodeEngine_() == false )
        {
            return false;
        }

        if( this->initializeFileEngine_() == false )
        {
            return false;
        }

		if( this->initializeConfigEngine_() == false )
		{
			return false;
		}

		if( this->initializeArchiveService_() == false )
		{
			return false;
		}

        if( this->initializeThreadEngine_() == false )
        {
            return false;
        }

        if( this->initializeParticleEngine2_() == false )
        {
            return false;
        }

        //	if( this->initializePhysicEngine2D_() == false )
        //	{
        //		return false;
        //	}

        if( this->initializeRenderEngine_() == false )
        {
            return false;
        }
                
        if( this->initializeSoundEngine_() == false )
        {
            if( this->initializeSilentSoundEngine_() == false )
            {
                return false;
            }
        }

        if( this->initializeScriptEngine_() == false )
        {
            return false;
        }

		if( this->initializeModuleEngine_() == false )
		{
			return false;
		}

		if( this->initializeDataManager_() == false )
		{
			return false;
		}

		if( this->initializeCacheManager_() == false )
		{
			return false;
		}

        if( this->initializeCodecEngine_() == false )
        {
            return false;
        }

        if( this->initializeInputEngine_() == false )
        {
            return false;
        }

        if( this->initializeApplicationService_() == false )
        {
            return false;
        }

		if( this->initializePrefetcherService_() == false )
		{
			return false;
		}

        if( this->initializePluginService_() == false )
        {
            return false;
        }

		m_timer = new SDLTimer;
		m_timer->initialize();

		m_input = new SDLInput;
		m_input->setServiceProvider( m_serviceProvider );
		
		if( m_input->initialize() == false )
		{
			return false;
		}
		
        {
            LOGGER_INFO(m_serviceProvider)( "initialize Image Codec..." );

			Menge::PluginInterface * pluginMengeImageCodec;
            initPluginMengeImageCodec( &pluginMengeImageCodec );
            pluginMengeImageCodec->initialize( m_serviceProvider );
			m_plugins.push_back( pluginMengeImageCodec );
        }

        {
            LOGGER_INFO(m_serviceProvider)( "initialize Sound Codec..." );

			Menge::PluginInterface * pluginMengeSoundCodec;
            initPluginMengeSoundCodec( &pluginMengeSoundCodec );
            pluginMengeSoundCodec->initialize( m_serviceProvider );
			m_plugins.push_back( pluginMengeSoundCodec );
        }

		{
			LOGGER_INFO(m_serviceProvider)( "initialize Ogg Vorbis Codec..." );

			Menge::PluginInterface * pluginMengeOggVorbis;
			initPluginMengeOggVorbis( &pluginMengeOggVorbis );
			pluginMengeOggVorbis->initialize( m_serviceProvider );
			m_plugins.push_back( pluginMengeOggVorbis );
		}

		{
			LOGGER_INFO(m_serviceProvider)( "initialize Video Codec..." );

			Menge::PluginInterface * pluginMengeVideoCodec;
			initPluginMengeVideoCodec( &pluginMengeVideoCodec );
			pluginMengeVideoCodec->initialize( m_serviceProvider );
			m_plugins.push_back( pluginMengeVideoCodec );
		}

		{
			LOGGER_INFO(m_serviceProvider)( "initialize Amplifier..." );

			Menge::PluginInterface * pluginMengeAmplifier;
			initPluginMengeAmplifier( &pluginMengeAmplifier );
			pluginMengeAmplifier->initialize( m_serviceProvider );
			m_plugins.push_back( pluginMengeAmplifier );
		}

		{
			LOGGER_INFO(m_serviceProvider)( "initialize Path Finder..." );

			Menge::PluginInterface * pluginPluginPathFinder;
			initPluginPathFinder( &pluginPluginPathFinder );
			pluginPluginPathFinder->initialize( m_serviceProvider );
			m_plugins.push_back( pluginPluginPathFinder );
		}

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

		String languagePack;
		Helper::s_getOption( " -lang:", m_commandLine, &languagePack );

		if( languagePack.empty() == true )
		{
			languagePack = CONFIG_VALUE(m_serviceProvider, "Locale", "Default", "en");
		}

        LOGGER_WARNING(m_serviceProvider)("Locale %s"
            , languagePack.c_str()
            );

		String personalityModule = CONFIG_VALUE(m_serviceProvider, "Game", "PersonalityModule", "Personality" );

		FilePath resourceIniPath;
		if( this->getApplicationPath_( "Resource", "Path", resourceIniPath ) == false )
		{
			return false;
		}

		Resolution defaultWindowResolution;
		this->getDesktopResolution( defaultWindowResolution );
		//Resolution defaultWindowResolution = CONFIG_VALUE(m_serviceProvider, "Window", "Size", Resolution(1024, 768));
		uint32_t defaultWindowBits = CONFIG_VALUE(m_serviceProvider, "Window", "Bits", 32U);
		bool defaultWindowFullscreen = CONFIG_VALUE(m_serviceProvider, "Window", "Fullscreen", true);
		bool defaultWindowVSync = CONFIG_VALUE(m_serviceProvider, "Window", "VSync", true);

		m_application->setDefaultWindowDescription( defaultWindowResolution, defaultWindowBits, defaultWindowFullscreen, defaultWindowVSync );

		GameServiceInterface * gameService = m_application->createGame();

		if( gameService == nullptr )
		{
			LOGGER_CRITICAL(m_serviceProvider)( "Application create game failed"
				);

			return false;
		}
		
        if( m_application->loadResourcePacks( ConstString::none(), resourceIniPath ) == false )
        {
            LOGGER_CRITICAL(m_serviceProvider)( "Application invalid load resource pak %s"
				, resourceIniPath.c_str()
                );

            return false;
        }

        LOGGER_INFO(m_serviceProvider)( "Application Initialize... %s"
            , m_platformName.c_str()
            );

		FilePath accountPath = STRINGIZE_STRING_LOCAL( m_serviceProvider, "accounts.ini" );

        LOGGER_INFO(m_serviceProvider)( "Initializing Game data... %s"
			, accountPath.c_str()
			);
				
        if( m_application->initializeGame( Helper::stringizeString(m_serviceProvider, personalityModule), Helper::stringizeString(m_serviceProvider, languagePack), accountPath, scriptInit ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Application invalid initialize game"
                );

            return false;
        }

		//int flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//SDL_DisplayMode mode;
		//SDL_GetCurrentDisplayMode( 0, &mode );

		Resolution windowResolution;
		m_application->calcWindowResolution( windowResolution );

		//m_window = SDL_CreateWindow( "Mengine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowResolution.getWidth(), windowResolution.getHeight(), flags );

        LOGGER_INFO(m_serviceProvider)( "Creating Render Window..." );

		if( m_application->createRenderWindow( m_window ) == false )
        {
            return false;
        }

		m_timer->reset();

		m_application->turnSound( true );

		m_input->updateSurfaceResolution( windowResolution.getWidth(), windowResolution.getHeight() );
		
        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	void SDLApplication::setActivate( bool _value )
	{
		m_active = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	void SDLApplication::handleEvent( const SDL_Event & _event )
	{		
		switch( _event.type )
		{
		case SDL_QUIT:
			{
				m_running = false;
			}break;
		case SDL_WINDOWEVENT:
			{
				if( _event.window.event == SDL_WINDOWEVENT_MINIMIZED )
				{
					m_active = false;
				}

				if( _event.window.event == SDL_WINDOWEVENT_RESTORED )
				{
					m_active = true;
				}

				if( _event.window.event == SDL_WINDOWEVENT_SHOWN )
				{
					m_active = true;
				}

				if( _event.window.event == SDL_WINDOWEVENT_HIDDEN )
				{
					m_active = false;
				}

				bool newFocus = m_focus;
				if( _event.window.event == SDL_WINDOWEVENT_FOCUS_LOST )
				{
					newFocus = false;
				}

				if( _event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED )
				{
					newFocus = true;
				}

				if( m_focus != newFocus )
				{
					m_focus = newFocus;
				}
			}break;

		case SDL_MOUSEWHEEL:
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_FINGERMOTION:
		case SDL_FINGERDOWN:
		case SDL_FINGERUP:
			{
				m_input->handleEvent( _event );
			}break;
		}

	}
    //////////////////////////////////////////////////////////////////////////
	void SDLApplication::loop()
    {
        if( m_application == nullptr )
        {
            return;
        }

		while( m_running == true )
        {
			float frameTime = m_timer->getDeltaTime();

			bool done = false;
			SDL_Event event;
			while( SDL_PollEvent( &event ) )
			{
				this->handleEvent( event );
			}

			if( m_active == false )
			{
				SDL_Delay( 100 );

				continue;
			}
            
            if( m_application->isFocus() == true )
            {
				//
            }
            
            bool updating = m_application->beginUpdate();

            if( updating == true )
            {                
                m_application->tick( frameTime );
            }
            else
            {
				SDL_Delay( 20 );
            }

			if( m_application->isFocus() == true )
			{
				if( m_application->render() == true )
				{
					m_application->flush();
				}
			}

			m_application->endUpdate();

			SDL_Delay( 0 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::openUrlInDefaultBrowser( const WString & _url )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLApplication::finalize()
    {
		if( m_timer != nullptr )
		{
			delete m_timer;
			m_timer = nullptr;
		}

		if( m_input != nullptr )
		{
			m_input->finalize();
			delete m_input;
			m_input = nullptr;
		}

		for( TVectorPlugins::iterator
			it = m_plugins.begin(),
			it_end = m_plugins.end();
		it != it_end;
		++it )
		{
			PluginInterface * plugin = *it;

			plugin->finalize();
		}

		if( m_application != nullptr )
		{
			m_application->finalize();
		}

		if( m_dataService != nullptr )
		{
			m_dataService->finalize();
			SERVICE_DESTROY( DataService, m_dataService );
		}

		if( m_moduleService != nullptr )
		{
			SERVICE_DESTROY( ModuleService, m_moduleService );
			m_moduleService = nullptr;
		}

		if( m_inputService != nullptr )
		{
			m_inputService->finalize();

			SERVICE_DESTROY( InputService, m_inputService );
			m_inputService = nullptr;
		}

		SERVICE_DESTROY( UnicodeService, m_unicodeService );        
		m_unicodeService = nullptr;

		SERVICE_DESTROY( UnicodeSystem, m_unicodeSystem );
		m_unicodeSystem = nullptr;

		if( m_fileService != nullptr )
		{            
			SERVICE_DESTROY( FileService, m_fileService );
			m_fileService = nullptr;
		}

		if( m_codecService != nullptr)
		{
			m_codecService->finalize();

			SERVICE_DESTROY( CodecService, m_codecService );
			m_codecService = nullptr;
		}

		//if( m_particleService != nullptr )
		//{
		//	SERVICE_DESTROY( ParticleService, m_particleService );
		//	m_particleService = nullptr;
		//}

		if( m_particleService2 != nullptr )
		{
			SERVICE_DESTROY( ParticleService2, m_particleService2 );
			m_particleService2 = nullptr;
		}

		if( m_soundService != nullptr )
		{
			m_soundService->finalize();

			SERVICE_DESTROY( SoundService, m_soundService );
			m_soundService = nullptr;
		}

		if( m_soundSystem != nullptr )
		{
			m_soundSystem->finalize();

			SERVICE_DESTROY( SoundSystem, m_soundSystem );        
			m_soundSystem = nullptr;
		}

		SERVICE_DESTROY( Application, m_application );
		m_application = nullptr;

		if( m_scriptService != nullptr )
		{
			m_scriptService->finalize();
		}

		if( m_renderService != nullptr )
		{
			m_renderService->finalize();
		}

		if( m_renderMaterialManager != nullptr )
		{
			m_renderMaterialManager->finalize();
		}

		if( m_renderTextureManager != nullptr )
		{
			m_renderTextureManager->finalize();
		}

		if( m_renderSystem != nullptr )
		{
			m_renderSystem->finalize();
		}

		if( m_renderService != nullptr )
		{
			SERVICE_DESTROY( RenderService, m_renderService );
			m_renderService = nullptr;
		}

		if( m_renderMaterialManager != nullptr )
		{
			SERVICE_DESTROY( RenderMaterialManager, m_renderMaterialManager );
			m_renderMaterialManager = nullptr;
		}

		if( m_renderTextureManager != nullptr )
		{
			SERVICE_DESTROY( RenderTextureManager, m_renderTextureManager );
			m_renderTextureManager = nullptr;
		}        

		if( m_renderSystem != nullptr )
		{
			SERVICE_DESTROY( RenderSystem, m_renderSystem );
			m_renderSystem = nullptr;
		}

		if( m_prefetcherService != nullptr )
		{
			m_prefetcherService->finalize();

			SERVICE_DESTROY( PrefetcherService, m_prefetcherService );
			m_prefetcherService = nullptr;
		}

		if( m_threadService != nullptr )
		{
			m_threadService->finalize();

			SERVICE_DESTROY( ThreadService, m_threadService );
			m_threadService = nullptr;
		}        

		if( m_threadSystem != nullptr )
		{
			m_threadSystem->finalize();

			SERVICE_DESTROY( ThreadSystem, m_threadSystem );
			m_threadSystem = nullptr;
		}

		if( m_scriptService != nullptr )
		{
			SERVICE_DESTROY( ScriptService, m_scriptService );
			m_scriptService = nullptr;
		}

		if( m_configService != nullptr )
		{
			SERVICE_DESTROY( ConfigService, m_configService );
			m_configService = nullptr;
		}

		if( m_stringizeService != nullptr )
		{
			SERVICE_DESTROY( StringizeService, m_stringizeService );
			m_stringizeService = nullptr;
		}

		if( m_fileLog != nullptr )
		{
			if( m_logService != nullptr )
			{
				m_logService->unregisterLogger( m_fileLog );
			}

			delete m_fileLog;
			m_fileLog = nullptr;
		}

		if( m_archiveService != nullptr )
		{
			SERVICE_DESTROY( ArchiveService, m_archiveService );
			m_archiveService = nullptr;
		}

		if( m_cacheService != nullptr )
		{
			m_cacheService->finalize();
			SERVICE_DESTROY( CacheService, m_cacheService );
			m_cacheService = nullptr;
		}

		for( TVectorPlugins::iterator
			it = m_plugins.begin(),
			it_end = m_plugins.end();
		it != it_end;
		++it )
		{
			PluginInterface * plugin = *it;

			plugin->destroy();
		}	

		m_plugins.clear();


		if( m_loggerConsole != nullptr )
		{
			if( m_logService != nullptr )
			{
				m_logService->unregisterLogger( m_loggerConsole );
			}

			delete m_loggerConsole;
			m_loggerConsole = nullptr;
		}

		if( m_notificationService != nullptr )
		{
			SERVICE_DESTROY( NotificationService, m_notificationService );
			m_notificationService = nullptr;
		}

		SERVICE_DESTROY( LogService, m_logService );
		m_logService = nullptr;

		SERVICE_DESTROY( ServiceProvider, m_serviceProvider );
		m_serviceProvider = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLApplication::stop()
    {
        m_running = false;
    }
	//////////////////////////////////////////////////////////////////////////
	const ConstString & SDLApplication::getPlatformName() const
	{
		return m_platformName;
	}
    //////////////////////////////////////////////////////////////////////////
	void SDLApplication::getDesktopResolution( Resolution & _resolution ) const
    {
		SDL_Surface * surface = SDL_GetWindowSurface( m_window );

		float width = static_cast<float>(surface->w);
		float height = static_cast<float>(surface->h);


        _resolution.setWidth( width );
        _resolution.setHeight( height );
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLApplication::minimizeWindow()
    {

    }
    //////////////////////////////////////////////////////////////////////////
	void SDLApplication::setHandleMouse( bool _handle )
    {

    }
    //////////////////////////////////////////////////////////////////////////
	void SDLApplication::setCursorPosition( const mt::vec2f & _pos )
    {

    }
    //////////////////////////////////////////////////////////////////////////
	void SDLApplication::showKeyboard()
    {
		if( m_input != nullptr )
		{
			m_input->showKeyboard( true );
		}
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLApplication::hideKeyboard()
    {
		if( m_input != nullptr )
		{
			m_input->showKeyboard( false );
		}
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLApplication::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
		
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLApplication::notifyVsyncChanged( bool _vsync )
    {

    }
    //////////////////////////////////////////////////////////////////////////
	void SDLApplication::notifyCursorModeChanged( bool _mode )
    {
	
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLApplication::notifyCursorClipping( const Viewport & _viewport )
    {

    }
    //////////////////////////////////////////////////////////////////////////
	void SDLApplication::notifyCursorUnClipping()
    {

    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::notifyCursorIconSetup( const ConstString & _name, const FilePath & _path, const Blobject & _buffer )
    {
		return true;
    }
    //////////////////////////////////////////////////////////////////////////
	bool SDLApplication::isDevelopmentMode() const
    {
        return m_developmentMode;
    }
    //////////////////////////////////////////////////////////////////////////
	void SDLApplication::onEvent( const ConstString & _event, const TMapParams & _params )
    {

    }
	//////////////////////////////////////////////////////////////////////////
	bool SDLApplication::createDirectoryUserPicture( const WString & _path, const WString & _file, const void * _data, size_t _size )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool SDLApplication::createDirectoryUserMusic( const WString & _path, const WString & _file, const void * _data, size_t _size )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t SDLApplication::getProcessHandleCount() const
	{
		return 0U;
	}
}
