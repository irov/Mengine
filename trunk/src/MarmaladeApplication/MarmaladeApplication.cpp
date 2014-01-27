#	include "Config/Config.h"

#	include "MarmaladeApplication.h"
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

#	include "Utils/Core/FileLogger.h"

//#	include "resource.h"

#	include "SimpleIni.h"

#	include "Core/File.h"

#	include <ctime>
#	include <algorithm>

#	include <sstream>
#	include <iomanip>

//#	include <mhook.h>
#	include "s3eDevice.h"
#	include "s3e.h"

#	include "StartupConfigLoader/StartupConfigLoader.h"

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

SERVICE_EXTERN(FileSystem, Menge::FileSystemInterface);
SERVICE_EXTERN(FileService, Menge::FileServiceInterface);

SERVICE_EXTERN(NotificationService, Menge::NotificationServiceInterface);
SERVICE_EXTERN(ScriptService, Menge::ScriptServiceInterface);

SERVICE_EXTERN(SoundSystem, Menge::SoundSystemInterface);
SERVICE_EXTERN(SilentSoundSystem, Menge::SoundSystemInterface);
SERVICE_EXTERN(SoundService, Menge::SoundServiceInterface);

SERVICE_EXTERN(AmplifierService, Menge::AmplifierServiceInterface);
SERVICE_EXTERN(InputService, Menge::InputServiceInterface);
SERVICE_EXTERN(ConverterService, Menge::ConverterServiceInterface);
SERVICE_EXTERN(CodecService, Menge::CodecServiceInterface);
SERVICE_EXTERN(PluginService, Menge::PluginServiceInterface);

SERVICE_EXTERN(ModuleService, Menge::ModuleServiceInterface);
SERVICE_EXTERN(DataService, Menge::DataServiceInterface);
SERVICE_EXTERN(CacheService, Menge::CacheServiceInterface);


extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    extern bool initPluginMengeImageCodec( Menge::PluginInterface ** _plugin );
    extern bool initPluginMengeSoundCodec( Menge::PluginInterface ** _plugin );
	extern bool initPluginMengeXmlCodec( Menge::PluginInterface ** _plugin );
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
        : m_running(true)
        , m_application(nullptr)
        , m_logService(nullptr)
        , m_fileService(nullptr)
        , m_inputService(nullptr)
        , m_marmaladeLayer(nullptr)
		, m_moduleService(nullptr)
		, m_dataService(nullptr)
		, m_cacheService(nullptr)
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

        if( m_application->initialize() == false )
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

        threadSystem->initialize();

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

        FileSystemInterface * fileSystem;
        if( SERVICE_CREATE( FileSystem, &fileSystem ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("WinApplication::initialize failed to create FileSystem"
                );

            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, fileSystem ) == false )
        {
            return false;
        }

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
        
        // mount root		
        ConstString c_dir = Helper::stringizeString(m_serviceProvider, "dir");
        if( m_fileService->mountFileGroup( ConstString::none(), ConstString::none(), m_currentPath, c_dir, false ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "WinApplication::setupFileService: failed to mount application directory %ls"
                , m_currentPath.c_str()
                );

            return false;
        }

        if( this->setupApplicationSetting_() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "WinApplication::setupFileService: failed to setup application %ls"
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
        if( m_fileService->mountFileGroup( Helper::stringizeString(m_serviceProvider, "user"), ConstString::none(), userPath, Helper::stringizeString(m_serviceProvider, "dir"), true ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "WinApplication: failed to mount user directory %ls"
                , m_userPath.c_str()
                );

            return false;
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
		if( m_fileService->mountFileGroup( c_dev, ConstString::none(), ConstString::none(), c_dir, false ) == false )
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

        if( m_soundService->initialize( false ) == false )
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

        if( m_soundService->initialize( true ) == false )
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
    bool MarmaladeApplication::initializeConverterEngine_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Codec Service..." );

        ConverterServiceInterface * converterService;
        if( SERVICE_CREATE( ConverterService, &converterService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, converterService ) == false )
        {
            return false;
        }

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
    bool MarmaladeApplication::initializeAmplifierService_()
    {
        LOGGER_INFO(m_serviceProvider)( "Initializing Amplifier Service..." );

        AmplifierServiceInterface * amplifierService;
        if( SERVICE_CREATE( AmplifierService, &amplifierService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, amplifierService ) == false )
        {
            return false;
        }

        m_amplifierService = amplifierService;


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
    bool MarmaladeApplication::setupApplicationSetting_()
    {
        StartupConfigLoader loader(m_serviceProvider);

        FilePath applicationPath = Helper::stringizeString( m_serviceProvider, "application.ini" );

        if( loader.load( ConstString::none(), applicationPath, m_settings ) == false )
        {
            return false;
        }

        m_projectName = m_settings.projectName;
        m_companyName = m_settings.companyName;

        String platformName;
        Helper::s_getOption( " -platform:", m_commandLine, &platformName );

        if( platformName.empty() == true )
        {
            platformName = "IPAD";
        }

        m_settings.applicationSettings.platformName = Helper::stringizeString( m_serviceProvider, platformName );
        m_settings.applicationSettings.baseDir = m_currentPath;

        if( m_projectName.empty() == true )
        {
            return false;
        }

        if( m_companyName.empty() == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::getMaxClientResolution( Resolution & _resolution ) const
    {
		int32 width = s3eSurfaceGetInt(S3E_SURFACE_WIDTH);
		int32 height = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT);

		_resolution.setWidth( width );
		_resolution.setHeight( height );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeApplication::initialize( const String & _commandLine )
    {
        m_commandLine = _commandLine;

        setlocale( LC_CTYPE, "" );
        //::timeBeginPeriod( 1 );

        String scriptInit;
        Helper::s_getOption( " -s:", m_commandLine, &scriptInit );

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

		m_timer = new MarmaladeTimer;
		m_timer->initialize();

		m_input = new MarmaladeInput;
		m_input->setServiceProvider( m_serviceProvider );
		m_input->initialize();

        if( this->initializeStringizeService_() == false )
        {
            return false;
        }

        if( this->initializeLogEngine_() == false )
        {
            return false;
        }

        if( this->initializeMarmaladeLayerService_() == false )
        {
            return false;
        }

        if( this->initializeArchiveService_() == false )
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

        if( this->initializeConverterEngine_() == false )
        {
            return false;
        }

        if( this->initializeInputEngine_() == false )
        {
            return false;
        }

        if( this->initializeAmplifierService_() == false )
        {
            return false;
        }

        if( this->initializeApplicationService_() == false )
        {
            return false;
        }

        if( this->initializePluginService_() == false )
        {
            return false;
        }
        
        //const WString & projectTitle = m_application->getProjectTitle();

        //const char * versionInfo = Application::getVersionInfo();
        //LOGGER_INFO(m_serviceProvider)( "SVN Revision: %s"
        //	, versionInfo
        //	);

		
		{
			LOGGER_INFO(m_serviceProvider)( "initialize Xml Codec..." );

			initPluginMengeXmlCodec( &m_pluginMengeXmlCodec );
			m_pluginMengeXmlCodec->initialize( m_serviceProvider );
		}

        {
            LOGGER_INFO(m_serviceProvider)( "initialize Image Codec..." );

            initPluginMengeImageCodec( &m_pluginMengeImageCodec );
            m_pluginMengeImageCodec->initialize( m_serviceProvider );
        }

        {
            LOGGER_INFO(m_serviceProvider)( "initialize Sound Codec..." );

            initPluginMengeSoundCodec( &m_pluginMengeSoundCodec );
            m_pluginMengeSoundCodec->initialize( m_serviceProvider );
        }

		if( m_application->setup( m_commandLine, m_settings.applicationSettings ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "Application setup failed" 
				);

			return false;
		}

        String languagePack = m_settings.defaultLocale;

        LOGGER_WARNING(m_serviceProvider)("Locale %s"
            , languagePack.c_str()
            );

        if( m_application->createGame( m_settings.personalityModule, Helper::stringizeString(m_serviceProvider, languagePack), m_settings.resourcePacks, m_settings.languagePacks ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Application create game failed"
                );

            return false;
        }

        LOGGER_INFO(m_serviceProvider)( "Application Initialize... %s"
            , m_settings.applicationSettings.platformName.c_str() 
            );

        LOGGER_INFO(m_serviceProvider)( "Initializing Game data..." );

        if( m_application->initializeGame( m_settings.appParams, scriptInit ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "Application invalid initialize game"
                );

            return false;
        }

        LOGGER_INFO(m_serviceProvider)( "Creating Render Window..." );

        if( m_application->createRenderWindow( 0 ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::loop()
    {
        if( m_application == nullptr )
        {
            return;
        }

        m_timer->reset();

		m_application->onTurnSound( true );
        
        while( true )
        {
            s3eBool quit = s3eDeviceCheckQuitRequest();

            if( quit == S3E_TRUE )
            {
                break;
            }
            
            s3eDeviceYield(0);

            bool rendered = false;

            if( m_application->isFocus() == true )
            {
                s3eDeviceBacklightOn();
                rendered = m_application->onRender();
            }

            m_input->update();
            
            bool updating = m_application->onUpdate();

            float frameTime = m_timer->getDeltaTime();

            if( updating == true )
            {                
                m_application->onTick( frameTime );
            }
            else
            {
                //Sleep?
            }

            if( rendered == true )
            {
                m_application->onFlush();
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeApplication::openUrlInDefaultBrowser( const WString & _url )
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeApplication::cmd( const WString & _command )
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

		if( m_input != nullptr )
		{
			delete m_input;
			m_input = nullptr;
		}
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::stop()
    {
        m_running = false;
        s3eDeviceRequestQuit();
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::getDesktopResolution( Resolution & _resolution ) const
    {
        int32 width = s3eSurfaceGetInt(S3E_SURFACE_WIDTH);
        int32 height = s3eSurfaceGetInt(S3E_SURFACE_HEIGHT);

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
		if( m_input != nullptr )
		{
			m_input->showKeyboard( true );
		}
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::hideKeyboard()
    {
		if( m_input != nullptr )
		{
			m_input->showKeyboard( false );
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
    void MarmaladeApplication::notifyCursorIconSetup( const FilePath & _name, void * _buffer, size_t _size )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MarmaladeApplication::isDevelopmentMode() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MarmaladeApplication::onEvent( const ConstString & _event, const TMapParams & _params )
    {

    }
    //////////////////////////////////////////////////////////////////////////
    size_t MarmaladeApplication::getMemoryUsage() const
    {
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void * MarmaladeApplication::checkpointMemory() const
    {
        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    size_t MarmaladeApplication::diffMemory( void * _checkpoint ) const
    {
        return 0;
    }

}
