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

//#	include "FileLogger.h"

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

SERVICE_EXTERN(ThreadSystem, Menge::ThreadSystemInterface);
SERVICE_EXTERN(ThreadService, Menge::ThreadServiceInterface);

SERVICE_EXTERN(ParticleSystem, Menge::ParticleSystemInterface);
SERVICE_EXTERN(ParticleService, Menge::ParticleServiceInterface);

SERVICE_EXTERN(RenderSystem, Menge::RenderSystemInterface);
SERVICE_EXTERN(RenderService, Menge::RenderServiceInterface);

SERVICE_EXTERN(PhysicSystem2D, Menge::PhysicSystem2DInterface);
SERVICE_EXTERN(PhysicService2D, Menge::PhysicService2DInterface);

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


extern "C" // only required if using g++
{
    //////////////////////////////////////////////////////////////////////////
    extern bool initPluginMengeImageCodec( Menge::PluginInterface ** _plugin );
    extern bool initPluginMengeSoundCodec( Menge::PluginInterface ** _plugin );
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
        static bool s_hasOption( const Menge::String& _option, const Menge::String& _commandLine )
        {
            if( _commandLine.find( _option ) == Menge::String::npos )
            {
                return false;
            }

            return true;
        }
    }

    MarmaladeApplication::MarmaladeApplication()
        : m_running(true)
        , m_application(NULL)
        , m_logService(NULL)
        , m_fileService(NULL)
        , m_inputService(0)
    {
    }

    MarmaladeApplication::~MarmaladeApplication()
    {
    }

    const String & MarmaladeApplication::getCurrentPath() const
    {
        static String empty;
        return empty;
    }


    size_t MarmaladeApplication::getShortPathName( const String & _path, char * _shortpath, size_t _shortpathlen )
    {
		return 0;
    }

    TimerInterface * MarmaladeApplication::getTimer( void ) const
    {
        return NULL;
    }

    bool MarmaladeApplication::initializeApplicationService_()
    {
		ApplicationInterface * application;
		if( createApplication( &application ) == false )
		{
			return false;
		}

		if( SERVICE_REGISTRY( m_serviceProvider, application ) == false )
		{
			return false;
		}

		m_application = application;

		if( m_application->initialize() == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "Application initialize failed" 
				);

			return false;
		}

		return true;
    }

    bool MarmaladeApplication::initializeNotificationService_()
    {
        LOGGER_INFO(m_serviceProvider)( "Inititalizing Notification Service..." );

        NotificationServiceInterface * notificationService;
        if( createNotificationService( &notificationService ) == false )
        {
            return false;
        }

        if( SERVICE_REGISTRY( m_serviceProvider, notificationService ) == false )
        {
            return false;
        }

        return true;
    }

    bool MarmaladeApplication::initializeThreadEngine_()
    {
		LOGGER_INFO(m_serviceProvider)( "Inititalizing Thread Service..." );

		ThreadSystemInterface * threadSystem;
		if( createThreadSystem( &threadSystem ) == false )
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
		if( createThreadService( &threadService ) == false )
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

    bool MarmaladeApplication::initializeFileEngine_()
    {

		return true;
    }



bool MarmaladeApplication::initializeStringizeService_()
{
	StringizeServiceInterface * stringizeService;
	if( createStringizeService( &stringizeService ) == false )
	{
		return false;
	}

	if( SERVICE_REGISTRY( m_serviceProvider, stringizeService ) == false )
	{
		return false;
	}

	return true;
}


bool MarmaladeApplication::initializeLogEngine_()
{
	LogServiceInterface * logService;
	if( createLogService( &logService ) == false )
	{
		return false;
	}

	if( SERVICE_REGISTRY( m_serviceProvider, logService ) == false )
	{
		return false;
	}



	m_logService = logService;

	m_logService->registerLogger( &m_loggerConsole );

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

bool MarmaladeApplication::initializeUnicodeEngine_()
{
	LOGGER_INFO(m_serviceProvider)( "Initializing Unicode Service..." );

	UnicodeSystemInterface * unicodeSystem;
	if( createUnicodeSystem( &unicodeSystem ) == false )
	{
		return false;
	}

	if( SERVICE_REGISTRY( m_serviceProvider, unicodeSystem ) == false )
	{
		return false;
	}

	UnicodeServiceInterface * unicodeService;
	if( createUnicodeService( &unicodeService ) == false )
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

bool MarmaladeApplication::initializeParticleEngine_()
{
	LOGGER_INFO(m_serviceProvider)( "Initializing Particle Service..." );

	ParticleSystemInterface * particleSystem;
	if( createParticleSystem( &particleSystem ) == false )
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
	if( createParticleService( &particleService ) == false )
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

bool MarmaladeApplication::initializePhysicEngine2D_()
{
	LOGGER_INFO(m_serviceProvider)( "Inititalizing Physics2D Service..." );

	PhysicSystem2DInterface * physicSystem2D;
	if( createPhysicSystem2D( &physicSystem2D ) == false )
	{
		LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializePhysicEngine2D_ Failed to initialize PhysicSystem2D"
			);

		return false;
	}

	if( SERVICE_REGISTRY( m_serviceProvider, physicSystem2D ) == false )
	{
		return false;
	}

	if( physicSystem2D->initialize() == false )
	{
		LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializePhysicEngine2D_ Failed to initialize Physics System 2D"
			);

		return false;
	}

	PhysicService2DInterface * physicService2D;
	if( createPhysicService2D( &physicService2D ) == false )
	{
		LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializePhysicEngine2D_ Failed to create Physic Service 2D"
			);

		return false;
	}

	if( SERVICE_REGISTRY( m_serviceProvider, physicService2D ) == false )
	{
		return false;
	}

	m_physicService2D = physicService2D;

	if( m_physicService2D->initialize() == false )
	{
		LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializePhysicEngine2D_ Failed to initialize Physics Service 2D"
			);

		return false;
	}

	return true;
}

bool MarmaladeApplication::initializeRenderEngine_()
{
	LOGGER_INFO(m_serviceProvider)( "Initializing Render Service..." );

	RenderSystemInterface * renderSystem;
	if( createRenderSystem( &renderSystem ) == false )
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
	if( createRenderService( &renderService ) == false )
	{
		return false;
	}

	if( SERVICE_REGISTRY( m_serviceProvider, renderService ) == false )
	{
		return false;
	}

	m_renderService = renderService;

	if( m_renderService->initialize( 8000 ) == false )
	{
		LOGGER_ERROR(m_serviceProvider)("MarmaladeApplication::initializeRenderEngine_ Failed to initialize Render Engine"
			);

		return false;
	}

	return true;
}

bool MarmaladeApplication::initializeSoundEngine_()
{
	LOGGER_INFO(m_serviceProvider)( "Initializing Sound Service..." );

	SoundSystemInterface * soundSystem;
	if( createSoundSystem( &soundSystem ) == false )
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
	if( createSoundService( &soundService ) == false )
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

bool MarmaladeApplication::initializeSilentSoundEngine_()
{
	LOGGER_INFO(m_serviceProvider)( "Initializing Silent Sound Service..." );

	SoundSystemInterface * soundSystem;
	if( createSilentSoundSystem( &soundSystem ) == false )
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
	if( createSoundService( &soundService ) == false )
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

bool MarmaladeApplication::initializeScriptEngine_()
{
	LOGGER_INFO(m_serviceProvider)( "Initializing Script Service..." );

	ScriptServiceInterface * scriptService;
	if( createScriptService( &scriptService ) == false )
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

bool MarmaladeApplication::initializeCodecEngine_()
{
	LOGGER_INFO(m_serviceProvider)( "Initializing Codec Service..." );

	CodecServiceInterface * codecService;
	if( createCodecService( &codecService ) == false )
	{
		return false;
	}

	if( SERVICE_REGISTRY( m_serviceProvider, codecService ) == false )
	{
		return false;
	}

	m_codecService = codecService;

	m_codecService->registerCodecExt( "png", Helper::stringizeString(m_serviceProvider, "pngImage") );
	m_codecService->registerCodecExt( "jpg", Helper::stringizeString(m_serviceProvider, "jpegImage") );
	m_codecService->registerCodecExt( "jpeg", Helper::stringizeString(m_serviceProvider, "jpegImage") );

	m_codecService->registerCodecExt( "ogg", Helper::stringizeString(m_serviceProvider, "oggSound") );
	m_codecService->registerCodecExt( "hit", Helper::stringizeString(m_serviceProvider, "hitPick") );


	return true;
}

bool MarmaladeApplication::initializeConverterEngine_()
{
	LOGGER_INFO(m_serviceProvider)( "Initializing Codec Service..." );

	ConverterServiceInterface * converterService;
	if( createConverterService( &converterService ) == false )
	{
		return false;
	}

	if( SERVICE_REGISTRY( m_serviceProvider, converterService ) == false )
	{
		return false;
	}

	return true;
}

bool MarmaladeApplication::initializeInputEngine_()
{
	LOGGER_INFO(m_serviceProvider)( "Initializing Input Service..." );

	InputServiceInterface * inputService;
	if( createInputService( &inputService ) == false )
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

bool MarmaladeApplication::initializeAmplifierService_()
{
	LOGGER_INFO(m_serviceProvider)( "Initializing Amplifier Service..." );

	AmplifierServiceInterface * amplifierService;
	if( createAmplifierService( &amplifierService ) == false )
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

bool MarmaladeApplication::initializePluginService_()
{
	LOGGER_INFO(m_serviceProvider)( "Initializing Plugin Service..." );

	PluginServiceInterface * pluginService;
	if( createPluginService( &pluginService ) == false )
	{
		return false;
	}

	if( SERVICE_REGISTRY(m_serviceProvider, pluginService) == false )
	{
		return false;
	}

	m_pluginService = pluginService;

	return true;
}

bool MarmaladeApplication::initializeNodeManager_()
{
	return true;
}

void MarmaladeApplication::setServiceProvider( ServiceProviderInterface * _serviceProvider )
{
    (void)_serviceProvider;
}
//////////////////////////////////////////////////////////////////////////
ServiceProviderInterface * MarmaladeApplication::getServiceProvider() const
{
    return m_serviceProvider;
}


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
        platformName = "Marmalade";
    }

    m_settings.applicationSettings.platformName = platformName;

    String utf8_currentPath;
    if( Helper::unicodeToUtf8( m_serviceProvider, m_currentPath, utf8_currentPath ) == false )
    {
        return false;
    }

    m_settings.applicationSettings.baseDir = utf8_currentPath;

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

void MarmaladeApplication::getMaxClientResolution( Resolution & _resolution ) const
{

}


bool MarmaladeApplication::initialize()
{
	setlocale( LC_CTYPE, "" );
	//::timeBeginPeriod( 1 );


	String scriptInit;
	Helper::s_getOption( " -s:", m_commandLine, &scriptInit );

	ServiceProviderInterface * serviceProvider;
	if( createServiceProvider( &serviceProvider ) == false )
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

	if( this->initializeNotificationService_() == false )
	{
		return false;
	}
	
//	if( this->initializeUnicodeEngine_() == false )
//	{
//		return false;
//	}
	
	if( this->initializeFileEngine_() == false )
	{
		return false;
	}
	
//	if( this->initializeThreadEngine_() == false )
//	{
//		return false;
//	}
	
//	if( this->initializeParticleEngine_() == false )
//	{
//		return false;
//	}
	
//	if( this->initializePhysicEngine2D_() == false )
//	{
//		return false;
//	}

	if( this->initializeRenderEngine_() == false )
	{
		return false;
	}
	
	//if( this->initializeSoundEngine_() == false )
	//{
	//	if( this->initializeSilentSoundEngine_() == false )
	//	{
	//		return false;
	//	}
	//}
	/*
	if( this->initializeScriptEngine_() == false )
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
	
	if( m_application->setup( m_commandLine, m_settings.applicationSettings ) == false )
	{
		LOGGER_ERROR(m_serviceProvider)( "Application setup failed" 
			);

		return false;
	}

	*/
	

	LOGGER_INFO(m_serviceProvider)( "Application Initialize... %s"
		, m_settings.applicationSettings.platformName.c_str() 
		);

	//const WString & projectTitle = m_application->getProjectTitle();
	
	//const char * versionInfo = Application::getVersionInfo();
	//LOGGER_INFO(m_serviceProvider)( "SVN Revision: %s"
	//	, versionInfo
	//	);

	//LOGGER_INFO(m_serviceProvider)( "Initializing Game data..." );
	
	//if( m_application->initializeGame( m_settings.appParams, scriptInit ) == false )
	//{
	//	LOGGER_ERROR(m_serviceProvider)( "Application invalid initialize game"
	//		);

	//	return false;
	//}
	
	return true;
}

void MarmaladeApplication::loop()
{
    if( m_application == 0 )
    {
        return;
    }

	s3eDeviceYield(0);

	bool rendered = false;
    
    if( m_application->isFocus() == true )
    {
		s3eDeviceBacklightOn();
        rendered = m_application->onRender();
    }
    
    bool updating = m_application->onUpdate();

    if( rendered )
    {
        m_application->onFlush();
    }
}

bool MarmaladeApplication::openUrlInDefaultBrowser( const WString & _url )
{
    return false;
}

void MarmaladeApplication::finalize()
{


}

void MarmaladeApplication::stop( void )
{
    m_running = false;
    s3eDeviceRequestQuit();
}

void MarmaladeApplication::getDesktopResolution( Resolution & _resolution ) const
{
}

void MarmaladeApplication::minimizeWindow( void )
{
}

void MarmaladeApplication::setHandleMouse( bool _handle )
{
}

void MarmaladeApplication::setCursorPosition( int _x, int _y )
{
}

void MarmaladeApplication::showKeyboard( void )
{
    
}

void MarmaladeApplication::hideKeyboard( void )
{
   
}

void MarmaladeApplication::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
{
}

void MarmaladeApplication::notifyVsyncChanged( bool _vsync )
{
}

void MarmaladeApplication::notifyCursorModeChanged( bool _mode )
{
}

void MarmaladeApplication::notifyCursorClipping( const Viewport & _viewport )
{
}

void MarmaladeApplication::notifyCursorUnClipping( void )
{
}


void MarmaladeApplication::notifyCursorIconSetup( const FilePath & _name, void * _buffer, size_t _size )
{
}

bool MarmaladeApplication::isDevelopmentMode() const
{
    return true;
}
}
