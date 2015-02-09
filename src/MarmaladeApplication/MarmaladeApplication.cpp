#	include "Config/Config.h"

#	include "MarmaladeApplication.h"
#	include "MarmaladeFileGroupDirectory.h"

#	include "Menge/Application.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/InputSystemInterface.h"

#	include <cstdio>
#	include <clocale>

//#	include "FPSMonitor.h"
//#	include "AlreadyRunningMonitor.h"
//#	include "CriticalErrorsMonitor.h"

#	include "Core/FileLogger.h"
#	include "Core/IniUtil.h"

//#	include "resource.h"

#	include <ctime>
#	include <algorithm>

#	include <sstream>
#	include <iomanip>

//#	include <mhook.h>
#	include "s3eDevice.h"
#	include "s3eWindow.h"
#	include "s3e.h"

#ifdef _MSC_VER
#	define snprintf _snprintf
#endif

SERVICE_EXTERN(ServiceProvider, Menge::ServiceProviderInterface);
SERVICE_EXTERN(Application, Menge::ApplicationInterface);
SERVICE_EXTERN(StringizeService, Menge::StringizeServiceInterface);
SERVICE_EXTERN(LogService, Menge::LogServiceInterface);
SERVICE_EXTERN(MarmaladeLayer, Menge::MarmaladeLayerInterface);

SERVICE_EXTERN(ArchiveService, Menge::ArchiveServiceInterface);

SERVICE_EXTERN(ThreadSystem, Menge::ThreadSystemInterface);
SERVICE_EXTERN(ThreadService, Menge::ThreadServiceInterface);

SERVICE_EXTERN(ParticleSystem, Menge::ParticleSystemInterface);
SERVICE_EXTERN(ParticleService, Menge::ParticleServiceInterface);

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


extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    extern bool initPluginMengeImageCodec( Menge::PluginInterface ** _plugin );
    extern bool initPluginMengeSoundCodec( Menge::PluginInterface ** _plugin );
	extern bool initPluginMengeVideoCodec( Menge::PluginInterface ** _plugin );
	extern bool initPluginMengeAmplifier( Menge::PluginInterface ** _plugin );
	extern bool initPluginMengeZip( Menge::PluginInterface ** _plugin );
	extern bool initPluginMengeLZ4( Menge::PluginInterface ** _plugin );

	extern bool initPluginMengeOggVorbis( Menge::PluginInterface ** _plugin );

	extern bool initPluginPathFinder( Menge::PluginInterface ** _plugin );
	
}

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    static const float s_activeFrameTime = 1000.f/60.f;
    static const float s_inactiveFrameTime = 100;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        static void s_getOption( const Menge::String& _option
            , const Menge::String& _commandLine
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
    MarmaladeApplication::MarmaladeApplication()
        : m_serviceProvider(nullptr)
        , m_application(nullptr)
        , m_marmaladeLayer(nullptr)
        , m_loggerConsole(nullptr)
        , m_fileLog(nullptr)
        , m_timer(nullptr)
		, m_marmaladeInput(nullptr)
		, m_running(false)
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
    MarmaladeApplication::~MarmaladeApplication()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const String & MarmaladeApplication::getCurrentPath() const
    {
        static String empty;
        return empty;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MarmaladeApplication::getShortPathName( const String & _path, char * _shortpath, size_t _shortpathlen )
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    TimerInterface * MarmaladeApplication::getTimer() const
    {
        return m_timer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeApplication::initializeApplicationService_()
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
    bool MarmaladeApplication::initializeNotificationService_()
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
    bool MarmaladeApplication::initializeThreadEngine_()
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
    bool MarmaladeApplication::initializeFileEngine_()
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

		m_fileService->registerFileGroupFactory( Helper::stringizeString(m_serviceProvider, "dir"), new FactorableUnique< FactoryDefault<MarmaladeFileGroupDirectory> >() );
        
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

#	ifndef MENGE_MASTER_RELEASE
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
	bool MarmaladeApplication::getApplicationPath_( const char * _section, const char * _key, ConstString & _path )
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
	bool MarmaladeApplication::initializeConfigEngine_()
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
    bool MarmaladeApplication::initializeStringizeService_()
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
    bool MarmaladeApplication::initializeLogEngine_()
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

        m_loggerConsole = new MarmaladeLogger();

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
    bool MarmaladeApplication::initializeMarmaladeLayerService_()
    {
        LOGGER_INFO(m_serviceProvider)( "Inititalizing MarmaladeLayer Service..." );

        MarmaladeLayerInterface * marmaladeLayer;
        if( SERVICE_CREATE( MarmaladeLayer, &marmaladeLayer ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeMarmaladeLayerService_ failed to create MarmaladeLayer"
                );

            return false;
        }

        if( SERVICE_REGISTRY(m_serviceProvider, marmaladeLayer) == false )
        {
            return false;
        }

        m_marmaladeLayer = marmaladeLayer;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeApplication::initializeArchiveService_()
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
    bool MarmaladeApplication::initializeUnicodeEngine_()
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
    bool MarmaladeApplication::initializeParticleEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Particle Service..." );

        ParticleSystemInterface * particleSystem;
        if( SERVICE_CREATE( ParticleSystem, &particleSystem ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializeParticleEngine_ Failed to initialize ParticleSystem"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, particleSystem ) == false )
        {
            return false;
        }

        ParticleServiceInterface * particleService;
        if( SERVICE_CREATE( ParticleService, &particleService ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializeParticleEngine_ Failed to initialize ParticleService"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, particleService ) == false )
        {
            return false;
        }

        m_particleService = particleService;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeApplication::initializePhysicEngine2D_()
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
    bool MarmaladeApplication::initializeRenderEngine_()
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

        if( m_renderService->initialize( 32000, 48000 ) == false )
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
    bool MarmaladeApplication::initializeSoundEngine_()
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
    bool MarmaladeApplication::initializeSilentSoundEngine_()
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
    bool MarmaladeApplication::initializeScriptEngine_()
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
	bool MarmaladeApplication::initializeModuleEngine_()
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
	bool MarmaladeApplication::initializeDataManager_()
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
	bool MarmaladeApplication::initializeCacheManager_()
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
	bool MarmaladeApplication::initializePrefetcherService_()
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
    bool MarmaladeApplication::initializeCodecEngine_()
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
    bool MarmaladeApplication::initializeInputEngine_()
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
    bool MarmaladeApplication::initializePluginService_()
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
    bool MarmaladeApplication::initializeNodeManager_()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        (void)_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * MarmaladeApplication::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::getMaxClientResolution( Resolution & _resolution ) const
    {
		int32 width = s3eSurfaceGetInt( S3E_SURFACE_WIDTH );
		int32 height = s3eSurfaceGetInt( S3E_SURFACE_HEIGHT );

		_resolution.setWidth( width );
		_resolution.setHeight( height );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeApplication::initialize( const String & _commandLine )
    {
		this->initializeMarmaladePauseCallback_();

        m_commandLine = " " + _commandLine + " ";

        setlocale( LC_CTYPE, "" );
        //::timeBeginPeriod( 1 );

        String scriptInit;
        Helper::s_getOption( " -s:", m_commandLine, &scriptInit );

		if( Helper::s_hasOption( " -dev ", m_commandLine ) == true )
		{
			m_developmentMode = true;
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

		int32 deviceClassID = s3eDeviceGetInt( S3E_DEVICE_CLASS );
		
		switch( deviceClassID )
		{
		case  S3E_DEVICE_CLASS_WINDOWS_GENERIC:
			{
				m_platformName = STRINGIZE_STRING_LOCAL(m_serviceProvider, "WIN");
			}break;
		case S3E_DEVICE_CLASS_OSX_DESKTOP:
			{
				m_platformName = STRINGIZE_STRING_LOCAL(m_serviceProvider, "OSX");
			}break;
		case S3E_DEVICE_CLASS_IPHONE:
			{
				m_platformName = STRINGIZE_STRING_LOCAL(m_serviceProvider, "IOS");
			}break;
		case S3E_DEVICE_CLASS_ANDROID_GENERIC:
			{
				m_platformName = STRINGIZE_STRING_LOCAL(m_serviceProvider, "ANDROID");
			}break;
		default:
			break;
		};
				
        if( this->initializeLogEngine_() == false )
        {
            return false;
        }

        if( this->initializeMarmaladeLayerService_() == false )
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

        if( this->initializeParticleEngine_() == false )
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

		s3eDeviceYield(0);
        
        if( this->initializeScriptEngine_() == false )
        {
            return false;
        }

		s3eDeviceYield(0);

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

		LOGGER_WARNING(m_serviceProvider)("Device info:"
			);

		const char * deviceID = s3eDeviceGetString( S3E_DEVICE_ID );

		LOGGER_WARNING(m_serviceProvider)("ID: %s"
			, deviceID
			);

		const char * deviceOS = s3eDeviceGetString( S3E_DEVICE_OS );

		LOGGER_WARNING(m_serviceProvider)("OS: %s"
			, deviceOS
			);

		const char * deviceOSVersion= s3eDeviceGetString( S3E_DEVICE_OS_VERSION );

		LOGGER_WARNING(m_serviceProvider)("OS Version: %s"
			, deviceOSVersion
			);

		const char * deviceClass = s3eDeviceGetString( S3E_DEVICE_CLASS );

		LOGGER_WARNING(m_serviceProvider)("Class: %s"
			, deviceClass
			);

		const char * deviceArchitecture = s3eDeviceGetString( S3E_DEVICE_ARCHITECTURE );

		LOGGER_WARNING(m_serviceProvider)("Architecture: %s"
			, deviceArchitecture
			);

		const char * deviceName = s3eDeviceGetString( S3E_DEVICE_NAME );

		LOGGER_WARNING(m_serviceProvider)("Name: %s"
			, deviceName
			);		
        
		m_timer = new MarmaladeTimer;
		m_timer->initialize();

		m_marmaladeInput = new MarmaladeInput;
		m_marmaladeInput->setServiceProvider( m_serviceProvider );
		m_marmaladeInput->initialize();
		
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

		s3eDeviceYield(0);
		
        if( m_application->createGame( Helper::stringizeString(m_serviceProvider, personalityModule), Helper::stringizeString(m_serviceProvider, languagePack), ConstString::none(), resourceIniPath ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Application create game failed"
                );

            return false;
        }

		s3eDeviceYield(0);

        LOGGER_INFO(m_serviceProvider)( "Application Initialize... %s"
            , m_platformName.c_str()
            );

		FilePath accountPath = STRINGIZE_STRING_LOCAL( m_serviceProvider, "accounts.ini" );

        LOGGER_INFO(m_serviceProvider)( "Initializing Game data... %s"
			, accountPath.c_str()
			);
				
        if( m_application->initializeGame( accountPath, scriptInit ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Application invalid initialize game"
                );

            return false;
        }

        LOGGER_INFO(m_serviceProvider)( "Creating Render Window..." );

        if( m_application->createRenderWindow( nullptr ) == false )
        {
            return false;
        }

#	ifndef _DEBUG		
		s3ePointerSetInt( S3E_POINTER_HIDE_CURSOR, 1 );
#	endif

		m_timer->reset();

		m_application->turnSound( true );

        return true;
    }
	//////////////////////////////////////////////////////////////////////////
	static int32 s3eCallback_Application_S3E_SURFACE_SCREENSIZE( void * _systemData, void * _userData )
	{
		s3eSurfaceOrientation * orientation = static_cast<s3eSurfaceOrientation *>(_systemData);

		ApplicationInterface * app = static_cast<ApplicationInterface *>(_userData);
		
		Resolution resolution;
		resolution.setWidth( orientation->m_Width );
		resolution.setHeight( orientation->m_Height );

		app->changeWindowResolution( resolution );

		return 0;
	}
    //////////////////////////////////////////////////////////////////////////
    static int32 s3eCallback_Input_S3E_SURFACE_SCREENSIZE( void * _systemData, void * _userData )
    {
        s3eSurfaceOrientation * orientation = static_cast<s3eSurfaceOrientation *>(_systemData);
        
        MarmaladeInput * input = static_cast<MarmaladeInput *>(_userData);
        
        input->updateSurfaceResolution();
        
        return 0;
    }
	//////////////////////////////////////////////////////////////////////////
	static int32 s3eCallback_UnPause( void * _systemData, void * _userData )
	{
		(void)_systemData;

		MarmaladeApplication * application = static_cast<MarmaladeApplication *>(_userData);

		application->setActivate( true );

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	static int32 s3eCallback_Pause( void * _systemData, void * _userData )
	{
		(void)_systemData;

		MarmaladeApplication * application = static_cast<MarmaladeApplication *>(_userData);

		application->setActivate( false );

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeApplication::setActivate( bool _value )
	{
		m_active = _value;
	}
	//////////////////////////////////////////////////////////////////////////
	void MarmaladeApplication::initializeMarmaladePauseCallback_()
	{
		m_active = true;

		bool successful = true;

		successful &= s3eDeviceRegister( S3E_DEVICE_UNPAUSE, &s3eCallback_UnPause, this ) == S3E_RESULT_SUCCESS;
		successful &= s3eDeviceRegister( S3E_DEVICE_FOREGROUND, &s3eCallback_UnPause, this ) == S3E_RESULT_SUCCESS;
		
		successful &= s3eDeviceRegister( S3E_DEVICE_BACKGROUND, &s3eCallback_Pause, this ) == S3E_RESULT_SUCCESS;
		successful &= s3eDeviceRegister( S3E_DEVICE_PAUSE, &s3eCallback_Pause, this ) == S3E_RESULT_SUCCESS;

		if( successful == false )
		{
			s3eDeviceUnRegister( S3E_DEVICE_UNPAUSE, &s3eCallback_UnPause );
			s3eDeviceUnRegister( S3E_DEVICE_FOREGROUND, &s3eCallback_UnPause );

			s3eDeviceUnRegister( S3E_DEVICE_BACKGROUND, &s3eCallback_Pause );
			s3eDeviceUnRegister( S3E_DEVICE_PAUSE, &s3eCallback_Pause );						
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::loop()
    {
        if( m_application == nullptr )
        {
            return;
        }

		if( s3eSurfaceRegister( S3E_SURFACE_SCREENSIZE, &s3eCallback_Application_S3E_SURFACE_SCREENSIZE, m_application ) != S3E_RESULT_SUCCESS )
		{
			const char * err_str = s3eSurfaceGetErrorString();
			s3eSurfaceError err = s3eSurfaceGetError();

			LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initialize S3E_SURFACE_SCREENSIZE %s [%d]"
				, err_str
				, err
				);
		}

		if( s3eSurfaceRegister( S3E_SURFACE_SCREENSIZE, &s3eCallback_Input_S3E_SURFACE_SCREENSIZE, m_marmaladeInput ) != S3E_RESULT_SUCCESS )
		{
			const char * err_str = s3eSurfaceGetErrorString();
			s3eSurfaceError err = s3eSurfaceGetError();

			LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initialize S3E_SURFACE_SCREENSIZE %s [%d]"
				, err_str
				, err
				);
		}

		if( s3eDeviceRegister( S3E_DEVICE_UNPAUSE, &s3eCallback_UnPause, m_application ) != S3E_RESULT_SUCCESS )
		{
			const char * err_str = s3eDeviceGetErrorString();
			s3eDeviceError err = s3eDeviceGetError();

			LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initialize S3E_DEVICE_UNPAUSE %s [%d]"
				, err_str
				, err
				);
		}

		if( s3eDeviceRegister( S3E_DEVICE_PAUSE, &s3eCallback_Pause, m_application ) != S3E_RESULT_SUCCESS )
		{
			const char * err_str = s3eDeviceGetErrorString();
			s3eDeviceError err = s3eDeviceGetError();

			LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initialize S3E_DEVICE_PAUSE %s [%d]"
				, err_str
				, err
				);
		}

		if( s3eDeviceRegister( S3E_DEVICE_FOREGROUND, &s3eCallback_UnPause, m_application ) != S3E_RESULT_SUCCESS )
		{
			const char * err_str = s3eDeviceGetErrorString();
			s3eDeviceError err = s3eDeviceGetError();

			LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initialize S3E_DEVICE_FOREGROUND %s [%d]"
				, err_str
				, err
				);
		}

		if( s3eDeviceRegister( S3E_DEVICE_BACKGROUND, &s3eCallback_Pause, m_application ) != S3E_RESULT_SUCCESS )
		{
			const char * err_str = s3eDeviceGetErrorString();
			s3eDeviceError err = s3eDeviceGetError();

			LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initialize S3E_DEVICE_BACKGROUND %s [%d]"
				, err_str
				, err
				);
		}

        while( true )
        {
            s3eBool quit = s3eDeviceCheckQuitRequest();

            if( quit == S3E_TRUE )
            {
                break;
            }

			if( m_active == false )
			{
				s3eDeviceYield( 20 );

				continue;
			}
            
            if( m_application->isFocus() == true )
            {
                s3eDeviceBacklightOn();
            }

            m_marmaladeInput->update();
            
            bool updating = m_application->beginUpdate();

            float frameTime = m_timer->getDeltaTime();

            if( updating == true )
            {                
                m_application->tick( frameTime );
            }
            else
            {
                s3eDeviceYield( 20 );
            }

			if( m_application->isFocus() == true )
			{
				if( m_application->render() == true )
				{
					m_application->flush();
				}
			}

			m_application->endUpdate();

			s3eDeviceYield( 0 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeApplication::openUrlInDefaultBrowser( const WString & _url )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::finalize()
    {
		if( m_timer != nullptr )
		{
			delete m_timer;
			m_timer = nullptr;
		}

		if( m_marmaladeInput != nullptr )
		{
			m_marmaladeInput->finalize();
			delete m_marmaladeInput;
			m_marmaladeInput = nullptr;
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

		if( m_particleService != nullptr )
		{
			SERVICE_DESTROY( ParticleService, m_particleService );
			m_particleService = nullptr;
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

		if( m_notificationService != nullptr )
		{
			SERVICE_DESTROY(NotificationService, m_notificationService);
			m_notificationService = nullptr;
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

		SERVICE_DESTROY( LogService, m_logService );
		m_logService = nullptr;

		SERVICE_DESTROY( ServiceProvider, m_serviceProvider );
		m_serviceProvider = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::stop()
    {
        m_running = false;
        s3eDeviceRequestQuit();
    }
	//////////////////////////////////////////////////////////////////////////
	const ConstString & MarmaladeApplication::getPlatformName() const
	{
		return m_platformName;
	}
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::getDesktopResolution( Resolution & _resolution ) const
    {
        int32 width = s3eSurfaceGetInt( S3E_SURFACE_WIDTH );
        int32 height = s3eSurfaceGetInt( S3E_SURFACE_HEIGHT );

        _resolution.setWidth( width );
        _resolution.setHeight( height );
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::minimizeWindow()
    {

    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::setHandleMouse( bool _handle )
    {

    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::setCursorPosition( const mt::vec2f & _pos )
    {

    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::showKeyboard()
    {
		if( m_marmaladeInput != nullptr )
		{
			m_marmaladeInput->showKeyboard( true );
		}
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::hideKeyboard()
    {
		if( m_marmaladeInput != nullptr )
		{
			m_marmaladeInput->showKeyboard( false );
		}
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
    {
		
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::notifyVsyncChanged( bool _vsync )
    {

    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::notifyCursorModeChanged( bool _mode )
    {
		if( _mode == true )
		{
			s3ePointerSetInt( S3E_POINTER_HIDE_CURSOR, 0 );
		}
		else
		{
			s3ePointerSetInt( S3E_POINTER_HIDE_CURSOR, 1 );
		}		
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::notifyCursorClipping( const Viewport & _viewport )
    {

    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::notifyCursorUnClipping()
    {

    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeApplication::notifyCursorIconSetup( const ConstString & _name, const FilePath & _path, const Blobject & _buffer )
    {
		return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeApplication::isDevelopmentMode() const
    {
        return m_developmentMode;
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::onEvent( const ConstString & _event, const TMapParams & _params )
    {

    }
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeApplication::createDirectoryUserPicture( const WString & _path, const WString & _file, const void * _data, size_t _size )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool MarmaladeApplication::createDirectoryUserMusic( const WString & _path, const WString & _file, const void * _data, size_t _size )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t MarmaladeApplication::getProcessHandleCount() const
	{
		return 0U;
	}
}
