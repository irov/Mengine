#	include "Config/Config.h"
#	include "Config/Stringstream.h"

#	include "WinApplication.h"

//#	include "Menge/Application.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/InputSystemInterface.h"
#	include "Interface/ConfigInterface.h"

#	include "WindowsLayer/VistaWindowsLayer.h"

#	include "Factory/FactorableUnique.h"
#	include "Factory/FactoryDefault.h"

#	include "Logger/Logger.h"

#	include <cstdio>
#	include <clocale>

#	include <shlobj.h>
#   include <errno.h>

#	include "ConsoleLogger.h"
#	include "MessageBoxLogger.h"

#	include "FPSMonitor.h"
#	include "AlreadyRunningMonitor.h"
#	include "CriticalErrorsMonitor.h"

#	include "Core/FileLogger.h"
#	include "Core/IniUtil.h"

#	include "resource.h"

#	include <ctime>
#	include <algorithm>

#	include <sstream>
#	include <iomanip>

#   include <WinBase.h>
#   include <Psapi.h>

#ifdef _MSC_VER
#	define snprintf _snprintf
#endif

SERVICE_EXTERN(ServiceProvider, Menge::ServiceProviderInterface);
SERVICE_EXTERN(Application, Menge::ApplicationInterface);
SERVICE_EXTERN(StringizeService, Menge::StringizeServiceInterface);
SERVICE_EXTERN(LogService, Menge::LogServiceInterface);
SERVICE_EXTERN(ConfigService, Menge::ConfigServiceInterface);

SERVICE_EXTERN(ArchiveService, Menge::ArchiveServiceInterface);
SERVICE_EXTERN(ModuleService, Menge::ModuleServiceInterface);

SERVICE_EXTERN(ThreadSystem, Menge::ThreadSystemInterface);
SERVICE_EXTERN(ThreadService, Menge::ThreadServiceInterface);

SERVICE_EXTERN(ParticleSystem, Menge::ParticleSystemInterface);
SERVICE_EXTERN(ParticleService, Menge::ParticleServiceInterface);
SERVICE_EXTERN(ParticleSystem2, Menge::ParticleSystemInterface2);
SERVICE_EXTERN(ParticleService2, Menge::ParticleServiceInterface2);

SERVICE_EXTERN(RenderSystem, Menge::RenderSystemInterface);
SERVICE_EXTERN(RenderService, Menge::RenderServiceInterface);
SERVICE_EXTERN(RenderTextureManager, Menge::RenderTextureServiceInterface);
SERVICE_EXTERN(RenderMaterialManager, Menge::RenderMaterialServiceInterface);

SERVICE_DUMMY(PhysicSystem, Menge::PhysicSystemInterface);

SERVICE_EXTERN(UnicodeSystem, Menge::UnicodeSystemInterface);
SERVICE_EXTERN(UnicodeService, Menge::UnicodeServiceInterface);

SERVICE_EXTERN(FileService, Menge::FileServiceInterface);

SERVICE_EXTERN(NotificationService, Menge::NotificationServiceInterface);
SERVICE_EXTERN(ScriptService, Menge::ScriptServiceInterface);

SERVICE_EXTERN(SoundSystem, Menge::SoundSystemInterface);
SERVICE_EXTERN(SilentSoundSystem, Menge::SoundSystemInterface);
SERVICE_EXTERN(SoundService, Menge::SoundServiceInterface);

SERVICE_EXTERN(PluginService, Menge::PluginServiceInterface);

SERVICE_EXTERN(InputService, Menge::InputServiceInterface);
SERVICE_EXTERN(ConverterService, Menge::ConverterServiceInterface);
SERVICE_EXTERN(CodecService, Menge::CodecServiceInterface);
SERVICE_EXTERN(DataService, Menge::DataServiceInterface);
SERVICE_EXTERN(CacheService, Menge::CacheServiceInterface);

SERVICE_EXTERN(HttpSystem, Menge::HttpSystemInterface);
SERVICE_EXTERN(PrefetcherService, Menge::PrefetcherServiceInterface); 

extern "C" // only required if using g++
{
	//////////////////////////////////////////////////////////////////////////
	extern bool initPluginMengeImageCodec( Menge::PluginInterface ** _plugin );
	extern bool initPluginMengeSoundCodec( Menge::PluginInterface ** _plugin );
	extern bool initPluginMengeAmplifier( Menge::PluginInterface ** _plugin );
	extern bool initPluginMengeVideoCodec( Menge::PluginInterface ** _plugin );
	extern bool initPluginMengeZip( Menge::PluginInterface ** _plugin );
	extern bool initPluginMengeLZ4( Menge::PluginInterface ** _plugin );	
	extern bool initPluginMengeOggVorbis( Menge::PluginInterface ** _plugin );
	extern bool initPluginMengeWin32FileGroup( Menge::PluginInterface ** _plugin );

	extern bool initPluginPathFinder( Menge::PluginInterface ** _plugin );
}

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	static const float s_activeFrameTime = 1000.f / 60.f;
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
	WinApplication::WinApplication() 
		: m_running(true)
		, m_active(false)
		, m_windowClassName(L"MengeWnd")
		, m_hWnd(NULL)
		, m_cursorInArea(false)
		, m_clickOutArea(false)
		, m_hInstance(NULL)
		, m_serviceProvider(nullptr)
		, m_loggerConsole(nullptr)
		, m_loggerMessageBox(nullptr)
		, m_application(nullptr)
		, m_fpsMonitor(nullptr)
		, m_alreadyRunningMonitor(0)
		, m_lastMouseX(0)
		, m_lastMouseY(0)
		, m_vsync(false)
		, m_maxfps(false)
		, m_cursorMode(false)
		, m_clipingCursor(FALSE)
		, m_deadKey('\0')
		, m_winTimer(nullptr)
		, m_isDoubleClick(false)
		, m_cursor(nullptr)
		, m_enableDebug(false)
		, m_developmentMode(false)
		, m_roamingMode(false)
		, m_nologsMode(false)
		, m_noDevPluginsMode(false)
		, m_muteMode(false)
		, m_nopause(false)
		, m_fileLog(nullptr)
		, m_profilerMode(false)
		, m_prefetcherService(nullptr)
		, m_inputService(nullptr)
		, m_unicodeService(nullptr)
		, m_logService(nullptr)
		, m_fileService(nullptr)
		, m_codecService(nullptr)
		, m_archiveService(nullptr)
		, m_threadSystem(nullptr)
		, m_threadService(nullptr)
		, m_particleService(nullptr)
		, m_particleService2(nullptr)
		, m_physicSystem(nullptr)
		, m_renderSystem(nullptr)
		, m_renderService(nullptr)
		, m_renderTextureManager(nullptr)
		, m_renderMaterialManager(nullptr)
		, m_stringizeService(nullptr)
		, m_soundSystem(nullptr)
		, m_soundService(nullptr)
		, m_scriptService(nullptr)
		, m_pluginService(nullptr)
		, m_converterService(nullptr)
		, m_moduleService(nullptr)
		, m_dataService(nullptr)
		, m_cacheService(nullptr)
		, m_httpSystem(nullptr)
		, m_configService(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	WinApplication::~WinApplication()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TimerInterface * WinApplication::getTimer() const
	{
		return m_winTimer;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t WinApplication::getShortPathName( const String & _path, char * _shortpath, size_t _shortpathlen )
	{
		WString unicode_path;
		Helper::utf8ToUnicode(m_serviceProvider, _path, unicode_path);

		wchar_t unicode_shortpath[MAX_PATH];
		DWORD unicode_shortpath_len = GetShortPathName( unicode_path.c_str(), unicode_shortpath, MAX_PATH );

		size_t utf8_shortpath_len;
		if( UNICODE_SERVICE(m_serviceProvider)
			->unicodeToUtf8( unicode_shortpath, unicode_shortpath_len, _shortpath, _shortpathlen, &utf8_shortpath_len ) == false )
		{
			return 0;
		}

		_shortpath[ utf8_shortpath_len ] = '\0';

		return utf8_shortpath_len;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initializeApplicationService_()
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

		if( m_application->initialize( m_commandLine ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Application initialize failed" 
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initializeNotificationService_()
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
	bool WinApplication::initializeThreadEngine_()
	{
		LOGGER_INFO(m_serviceProvider)( "Inititalizing Thread Service..." );

		ThreadSystemInterface * threadSystem;
		if( SERVICE_CREATE( ThreadSystem, &threadSystem ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeThreadEngine_ failed to create ThreadSystem"
				);

			return false;
		}

		if( SERVICE_REGISTRY(m_serviceProvider, threadSystem ) == false )
		{
			return false;
		}

		m_threadSystem = threadSystem;

		if( m_threadSystem->initialize() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeThreadEngine_ failed to initialize ThreadSystem"
				);

			return false;
		}

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

		if( m_threadService->initialize( 16 ) == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initializeArchiveService_()
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
			LOGGER_INFO(m_serviceProvider)("Initialize Zip...");
			PluginInterface * plugin;
			initPluginMengeZip( &plugin );
			plugin->initialize( m_serviceProvider );
			m_plugins.push_back( plugin );
		}

		{
			LOGGER_INFO(m_serviceProvider)("Initialize LZ4...");
			PluginInterface * plugin;
			initPluginMengeLZ4( &plugin );
			plugin->initialize( m_serviceProvider );
			m_plugins.push_back( plugin );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::getApplicationPath_( const char * _section, const char * _key, ConstString & _path )
	{
		FilePath applicationPath = CONST_STRING_LOCAL( m_serviceProvider, "application.ini" );

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
	bool WinApplication::initializeConfigEngine_()
	{
		LOGGER_WARNING(m_serviceProvider)("Inititalizing Config Manager..." );

		ConfigServiceInterface * configService;

		if( SERVICE_CREATE( ConfigService, &configService ) == false )
		{
			return false;
		}

		SERVICE_REGISTRY( m_serviceProvider, configService );

		m_configService = configService;

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
	bool WinApplication::initializeFileEngine_()
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
			LOGGER_ERROR(m_serviceProvider)("WinApplication::setupFileService: failed to initialize fileService"
				);

			return false;
		}

		{
			LOGGER_INFO(m_serviceProvider)("Initialize Win32 file group...");
			PluginInterface * plugin;
			initPluginMengeWin32FileGroup( &plugin );
			plugin->initialize( m_serviceProvider );
			m_plugins.push_back( plugin );
		}

		if( m_enableDebug == false )
		{
			m_windowsLayer->setModuleCurrentDirectory();
		}

		if( m_windowsLayer->getCurrentDirectory( m_currentPath ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::setupFileService: failed to get current directory"
				);

			return false;
		}

		String utf8_currentPath;        
		if( Helper::unicodeToUtf8( m_serviceProvider, m_currentPath, utf8_currentPath ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::setupFileService: failed to convert %ls to utf8"
				, m_currentPath.c_str()
				);

			return false;
		}

		LOGGER_WARNING(m_serviceProvider)("Current Path %ls"
			, m_currentPath.c_str()
			);

		FilePath currentPath = Helper::stringizeString( m_serviceProvider, utf8_currentPath );
		// mount root		
		if( m_fileService->mountFileGroup( ConstString::none(), currentPath, CONST_STRING_LOCAL(m_serviceProvider, "dir") ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::setupFileService: failed to mount application directory %ls"
				, m_currentPath.c_str()
				);

			return false;
		}

#	ifndef MENGE_MASTER_RELEASE
		// mount root		
		if( m_fileService->mountFileGroup( CONST_STRING_LOCAL(m_serviceProvider, "dev"), ConstString::none(), CONST_STRING_LOCAL(m_serviceProvider, "dir") ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::setupFileService: failed to mount dev directory %ls"
				, m_currentPath.c_str()
				);

			return false;
		}
#   endif

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initializeUserDirectory_()
	{
		//m_tempPath.clear();
		m_userPath.clear();

		if( m_developmentMode == true && m_roamingMode == false )
		{			
			m_userPath += m_currentPath;
			m_userPath += L"User";
			m_userPath += MENGE_FOLDER_DELIM;
		}
		else	// create user directory in ~/Local Settings/Application Data/<uUserPath>/
		{
			WChar buffer[MAX_PATH];
			LPITEMIDLIST itemIDList;

			HRESULT hr = SHGetSpecialFolderLocation( NULL,
				CSIDL_APPDATA | CSIDL_FLAG_CREATE, &itemIDList );

			if( hr != S_OK )
			{
				WString msg;

				if( m_windowsLayer->makeFormatMessage( hr, msg ) == false )
				{
					LOGGER_ERROR(m_serviceProvider)("SHGetSpecialFolderLocation invalid %d"
						, hr
						);
				}
				else
				{
					LOGGER_ERROR(m_serviceProvider)("SHGetSpecialFolderLocation invalid %ls '%d'"
						, msg.c_str()
						, hr
						);
				}

				return false;
			}

			BOOL result = SHGetPathFromIDListW( itemIDList, buffer );

			if( result == FALSE )
			{
				LOGGER_ERROR(m_serviceProvider)("SHGetPathFromIDListW invalid"
					);

				return false;
			}

			CoTaskMemFree( itemIDList );

			m_userPath = buffer;
			m_userPath += MENGE_FOLDER_DELIM;
			m_userPath += CONFIG_VALUE(m_serviceProvider, "Project", "Company", L"NONAME");
			m_userPath += MENGE_FOLDER_DELIM;
			m_userPath += CONFIG_VALUE(m_serviceProvider, "Project", "Name", L"UNKNOWN");
			m_userPath += MENGE_FOLDER_DELIM;
		}

		String utf8_userPath;
		if( Helper::unicodeToUtf8( m_serviceProvider, m_userPath, utf8_userPath ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication: failed user directory %ls convert to ut8f"
				, m_userPath.c_str()
				);

			return false;
		}

		FilePath userPath = Helper::stringizeString( m_serviceProvider, utf8_userPath );

		// mount user directory
		if( m_fileService->mountFileGroup( CONST_STRING_LOCAL(m_serviceProvider, "user"), userPath, CONST_STRING_LOCAL(m_serviceProvider, "dir") ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication: failed to mount user directory %ls"
				, m_userPath.c_str()
				);

			return false;
		}

		if( m_fileService->existDirectory( CONST_STRING_LOCAL(m_serviceProvider, "user"), ConstString::none() ) == false )
		{
			m_fileService->createDirectory( CONST_STRING_LOCAL(m_serviceProvider, "user"), ConstString::none() );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initializeLogFile_()
	{
		if( m_nologsMode == true )
		{
			return true;
		}

		std::time_t ctTime; 
		std::time(&ctTime);
		std::tm* sTime = std::localtime( &ctTime );

		WStringstream dateStream;
		dateStream << 1900 + sTime->tm_year 
			<< L"_" << std::setw(2) << std::setfill(L'0') << (sTime->tm_mon+1) 
			<< L"_" << std::setw(2) << std::setfill(L'0') << sTime->tm_mday 
			<< L"_" << std::setw(2) << std::setfill(L'0') << sTime->tm_hour 
			<< L"_" << std::setw(2) << std::setfill(L'0') << sTime->tm_min 
			<< L"_"	<< std::setw(2) << std::setfill(L'0') << sTime->tm_sec;

		WString date = dateStream.str();

		WString unicode_logFilename;
		unicode_logFilename += L"Game";

		if( m_developmentMode == true && m_roamingMode == false )
		{
			unicode_logFilename += L"_";
			unicode_logFilename += date;
		}

		unicode_logFilename += L".log";

		String utf8_logFilename;
		if( Helper::unicodeToUtf8( m_serviceProvider, unicode_logFilename, utf8_logFilename ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication: failed log directory %ls convert to ut8f"
				, unicode_logFilename.c_str()
				);

			return false;
		}

		FilePath logFilename = Helper::stringizeString( m_serviceProvider, utf8_logFilename );

		OutputStreamInterfacePtr fileLogInterface = 
			m_fileService->openOutputFile( CONST_STRING_LOCAL(m_serviceProvider, "user"), logFilename );

		if( fileLogInterface != nullptr )
		{
			m_fileLog = new FileLogger(fileLogInterface);

			m_logService->registerLogger( m_fileLog );

			LOGGER_INFO(m_serviceProvider)("WinApplication: Starting log to %s"
				, logFilename.c_str()
				);
		}

		WString logPath;
		logPath += m_userPath;
		logPath += unicode_logFilename;

		WString dumpPath;
		dumpPath += m_userPath;
		dumpPath += L"Dump";
		dumpPath += L"_";
		dumpPath += date;
		dumpPath += L".dmp";

		CriticalErrorsMonitor::run( logPath, dumpPath );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initializeStringizeService_()
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
	bool WinApplication::initializeLogEngine_()
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

		if( Helper::s_hasOption( " -console ", m_commandLine ) == true && m_nologsMode == false )
		{
			m_loggerConsole = new ConsoleLogger(m_serviceProvider);

			if( m_windowsType != EWT_98 )
			{
				m_loggerConsole->createConsole();
			}

			m_logService->registerLogger( m_loggerConsole );
		}

		if( m_nologsMode == false )
		{
			m_loggerMessageBox = new MessageBoxLogger(m_serviceProvider);

			m_loggerMessageBox->setVerboseLevel( LM_CRITICAL );

			m_logService->registerLogger( m_loggerMessageBox );
		}
				
		EMessageLevel m_logLevel;

		if( m_developmentMode == true && m_roamingMode == false )
		{
			m_logLevel = LM_LOG;
		}
		else
		{
			m_logLevel = LM_ERROR;
		}

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
		else if ( logLevel == "4" )
		{
			m_logLevel = LM_CRITICAL;
		}
		else if ( logLevel == "5" )
		{
			m_logLevel = LM_FATAL;
		}

		m_logService->setVerboseLevel( m_logLevel );

		uint32_t verboseFlag = 0;

		if( m_profilerMode )
		{
			verboseFlag |= 0x00000001;
		}

		m_logService->setVerboseFlag( verboseFlag );

		if( Helper::s_hasOption( " -verbose ", m_commandLine ) == true )
		{
			m_logService->setVerboseLevel( LM_MAX );

			LOGGER_INFO(m_serviceProvider)( "Verbose logging mode enabled" );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initializeUnicodeEngine_()
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
	bool WinApplication::initializeParticleEngine_()
	{
		LOGGER_INFO(m_serviceProvider)( "Initializing Particle Service..." );

		ParticleServiceInterface * particleService;
		if( SERVICE_CREATE( ParticleService, &particleService ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeParticleEngine_ Failed to initialize ParticleService"
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
	bool WinApplication::initializeParticleEngine2_()
	{
		LOGGER_INFO(m_serviceProvider)( "Initializing Particle Service 2..." );

		ParticleServiceInterface2 * particleService;
		if( SERVICE_CREATE( ParticleService2, &particleService ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeParticleEngine2_ Failed to initialize ParticleService2"
				);

			return false;
		}

		if( particleService->initialize() == false )
		{
			return false;
		}

		if( SERVICE_REGISTRY( m_serviceProvider, particleService ) == false )
		{
			return false;
		}

		m_particleService2 = particleService;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initializePhysicEngine2D_()
	{
		LOGGER_INFO(m_serviceProvider)( "Inititalizing Physics2D Service..." );

		PhysicSystemInterface * physicSystem;
		if( SERVICE_CREATE( PhysicSystem, &physicSystem ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializePhysicEngine2D_ Failed to initialize PhysicSystem2D"
				);

			return false;
		}

		if( SERVICE_REGISTRY( m_serviceProvider, physicSystem ) == false )
		{
			return false;
		}

		if( physicSystem->initialize() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializePhysicEngine2D_ Failed to initialize Physics System 2D"
				);

			return false;
		}

		m_physicSystem = physicSystem;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initializeRenderEngine_()
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
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeRenderEngine_ Failed to initialize Render System"
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
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeRenderEngine_ Failed to initialize Render Engine"
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

		RenderMaterialInterfacePtr debugMaterial = m_renderMaterialManager
			->getMaterial( CONST_STRING_LOCAL(m_serviceProvider, "Debug"), false, false, PT_LINELIST, 0, nullptr );

		m_renderService->setDebugMaterial( debugMaterial );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initializeSoundEngine_()
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
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeSoundEngine_ Failed to initialize Sound System"
				);

			return false;
		}

		m_soundSystem = soundSystem;

		SoundServiceInterface * soundService;
		if( SERVICE_CREATE( SoundService, &soundService ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeSoundEngine_ Failed to create Sound Engine"
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
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeSoundEngine_ Failed to initialize Sound Engine"
				);

			m_serviceProvider->unregistryService( "SoundService" );

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initializeSilentSoundEngine_()
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
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeSoundEngine_ Failed to initialize Silent Sound System"
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

		if( m_soundService->initialize( true, true ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeSilentSoundEngine_ Failed to initialize Sound Engine"
				);

			return false;                
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initializeScriptEngine_()
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
			LOGGER_ERROR(m_serviceProvider)("WinApplication::initializeScriptEngine_ Failed to initialize Script Engine"
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initializeModuleEngine_()
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
	bool WinApplication::initializeCodecEngine_()
	{
		LOGGER_INFO(m_serviceProvider)( "Initializing Codec Service..." );

		CodecServiceInterface * codecService;
		if( SERVICE_CREATE( CodecService, &codecService ) == false )
		{
			return false;
		}

		if( codecService->initialize() == false )
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
	bool WinApplication::initializeDataManager_()
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
	bool WinApplication::initializeCacheManager_()
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
	bool WinApplication::initializeHttpService_()
	{
		LOGGER_INFO(m_serviceProvider)( "Inititalizing Http Service..." );

		HttpSystemInterface * httpSystem;

		if( SERVICE_CREATE( HttpSystem, &httpSystem ) == false )
		{
			return false;
		}

		SERVICE_REGISTRY( m_serviceProvider, httpSystem );

		if( httpSystem->initialize() == false )
		{
			return false;
		}

		m_httpSystem = httpSystem;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initializePrefetcherService_()
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
	bool WinApplication::initializeConverterEngine_()
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

		m_converterService = converterService;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initializeInputEngine_()
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
	bool WinApplication::initializePluginService_()
	{
		LOGGER_INFO(m_serviceProvider)( "Initializing Plugin Service..." );

		PluginServiceInterface * pluginService;
		if( SERVICE_CREATE( PluginService, &pluginService ) == false )
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
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		(void)_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * WinApplication::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initialize( HINSTANCE _hInstance, const String& _commandLine )
	{	
		m_hInstance = _hInstance;
		m_commandLine = " " + _commandLine + " ";

		setlocale( LC_CTYPE, "" );
		//::timeBeginPeriod( 1 );

#	ifdef _MSC_VER
		m_enableDebug = true;
#	else
		m_enableDebug = false;
#	endif

		if( Helper::s_hasOption( " -maxfps ", m_commandLine ) == true )
		{
			m_maxfps = true;
		}

		if( Helper::s_hasOption( " -dev ", m_commandLine ) == true )
		{
			m_developmentMode = true;
		}

		if( Helper::s_hasOption( " -profiler ", m_commandLine ) == true )
		{
			m_profilerMode = true;
		}

		if( Helper::s_hasOption( " -nodevplugins ", m_commandLine ) == true )
		{
			m_noDevPluginsMode = true;
		}

		if( Helper::s_hasOption( " -mute ", m_commandLine ) == true )
		{
			m_muteMode = true;
		}

		if( Helper::s_hasOption( " -nopause ", m_commandLine ) == true )
		{
			m_nopause = true;
		}

		if( Helper::s_hasOption( " -roaming ", m_commandLine ) == true )
		{
			m_roamingMode = true;
		}

		if( Helper::s_hasOption( " -nologs ", m_commandLine ) == true )
		{
			m_nologsMode = true;
		}

		String scriptInit;
		Helper::s_getOption( " -s:", m_commandLine, &scriptInit );

		ServiceProviderInterface * serviceProvider;
		if( SERVICE_CREATE( ServiceProvider, &serviceProvider ) == false )
		{
			return false;
		}

		m_serviceProvider = serviceProvider;

		if( SERVICE_REGISTRY( m_serviceProvider, this ) == false )
		{
			return false;
		}

		m_windowsLayer = new VistaWindowsLayer();

		if( SERVICE_REGISTRY( m_serviceProvider, m_windowsLayer ) == false )
		{
			return false;
		}

		m_windowsType = m_windowsLayer->getWindowsType();

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

		if( this->initializeUserDirectory_() == false )
		{
			return false;
		}

		if( this->initializeLogFile_() == false )
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

		if( this->initializeParticleEngine2_() == false )
		{
			return false;
		}

		//if( this->initializePhysicEngine2D_() == false )
		//{
		//    return false;
		//}

		if( this->initializeRenderEngine_() == false )
		{
			return false;
		}

		if( m_muteMode == true || this->initializeSoundEngine_() == false )
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

		if( this->initializeCodecEngine_() == false )
		{
			return false;
		}

		if( this->initializeDataManager_() == false )
		{
			return false;
		}

		if( this->initializeHttpService_() == false )
		{
			return false;
		}

		if( this->initializePrefetcherService_() == false )
		{
			return false;
		}

		if( this->initializeCacheManager_() == false )
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

		if( this->initializeApplicationService_() == false )
		{
			return false;
		}

		if( this->initializePluginService_() == false )
		{
			return false;
		}

		m_winTimer = new WinTimer();
		m_winTimer->initialize();

		// seed randomizer
		LARGE_INTEGER randomSeed;
		::QueryPerformanceCounter(&randomSeed);
		srand( randomSeed.LowPart );

		LOGGER_WARNING(m_serviceProvider)( "initialize Application..." );
		LOGGER_WARNING(m_serviceProvider)( "command: '%s'"
			, m_commandLine.c_str()
			);

		if( Helper::s_hasOption( " -noaccounts ", m_commandLine ) == true )
		{
			CONFIG_SET(m_serviceProvider, "Development", "NoAccount", "1");
		}

		if( Helper::s_hasOption( " -novideo ", m_commandLine ) == true )
		{
			CONFIG_SET(m_serviceProvider, "Development", "NoVideo", "1");
		}

#	define MENGINE_ADD_PLUGIN( Init, Info )\
	{\
		PluginInterface * plugin;\
		Init( &plugin );\
		if( plugin->initialize( m_serviceProvider ) == false )\
		{\
			LOGGER_ERROR(m_serviceProvider)( "Invalid %s", Info );\
			plugin->destroy();\
		}\
		else\
		{\
			LOGGER_INFO(m_serviceProvider)( Info );\
			m_plugins.push_back( plugin );\
		}\
	}		

		MENGINE_ADD_PLUGIN( initPluginMengeImageCodec, "initialize Plugin Image Codec..." );
		MENGINE_ADD_PLUGIN( initPluginMengeSoundCodec, "initialize Plugin Sound Codec..." );
		MENGINE_ADD_PLUGIN( initPluginMengeOggVorbis, "initialize Plugin Ogg Vorbis Codec..." );
		MENGINE_ADD_PLUGIN( initPluginMengeAmplifier, "initialize Plugin Amplifier..." );
		MENGINE_ADD_PLUGIN( initPluginMengeVideoCodec, "initialize Plugin Video Codec..." );
		MENGINE_ADD_PLUGIN( initPluginPathFinder, "initialize Plugin Path Finder..." );

#	undef MENGINE_ADD_PLUGIN

		TVectorWString plugins;
		CONFIG_VALUES(m_serviceProvider, "Plugins", "Name", plugins);

		for( TVectorWString::const_iterator
			it = plugins.begin(),
			it_end = plugins.end();
		it != it_end;
		++it )
		{
			const WString & pluginName = *it;

			PluginInterface * plugin = PLUGIN_SERVICE(m_serviceProvider)
				->loadPlugin( pluginName );

			if( plugin == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("Application Failed to load plugin %ls"
					, pluginName.c_str()
					);

				return false;
			}
		}

		if( m_noDevPluginsMode == false )
		{
			TVectorWString devPlugins;
			CONFIG_VALUES(m_serviceProvider, "DevPlugins", "Name", devPlugins);

			for( TVectorWString::const_iterator
				it = devPlugins.begin(),
				it_end = devPlugins.end();
			it != it_end;
			++it )
			{
				const WString & pluginName = *it;

				PluginInterface * plugin = PLUGIN_SERVICE(m_serviceProvider)
					->loadPlugin( pluginName );

				if( plugin == nullptr )
				{
					LOGGER_WARNING(m_serviceProvider)("Application Failed to load dev plugin %ls"
						, pluginName.c_str()
						);
				}
			}
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

		Resolution defaultWindowResolution = CONFIG_VALUE(m_serviceProvider, "Window", "Size", Resolution(1024, 768));
		uint32_t defaultWindowBits = CONFIG_VALUE(m_serviceProvider, "Window", "Bits", 32U);
		bool defaultWindowFullscreen = CONFIG_VALUE(m_serviceProvider, "Window", "Fullscreen", true);
		bool defaultWindowVSync = CONFIG_VALUE(m_serviceProvider, "Window", "VSync", true);

		m_application->setDefaultWindowDescription( defaultWindowResolution, defaultWindowBits, defaultWindowFullscreen, defaultWindowVSync );

		if( m_application->createGame( Helper::stringizeString(m_serviceProvider, personalityModule), Helper::stringizeString(m_serviceProvider, languagePack), ConstString::none(), resourceIniPath ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Application create game failed"
				);

			return false;
		}

		LOGGER_INFO(m_serviceProvider)( "Application Initialize... %s"
			, m_application->getPlatformName().c_str()
			);

		const ConstString & projectTitle = m_application->getProjectTitle();

		WString wprojectTitle;
		if( Helper::utf8ToUnicodeSize(m_serviceProvider, projectTitle.c_str(), projectTitle.size(), wprojectTitle ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Application project title %s not convert to unicode"
				, projectTitle.c_str()
				);
		}

		bool screenSaverMode = this->isSaverRunning();

		if( screenSaverMode == true )
		{
			LOGGER_ERROR(m_serviceProvider)("Application is saver running"
				);

			String lowerCmdLine = m_commandLine.substr();
			std::transform( lowerCmdLine.begin(), lowerCmdLine.end(), lowerCmdLine.begin(), std::ptr_fun( ::tolower ) );

			if( lowerCmdLine.find(" /p") != String::npos || m_commandLine.find(" -p") != String::npos )
			{
				return false;
			}
			else if( lowerCmdLine.find(" /c") != String::npos || m_commandLine.find(" -c") != String::npos )
			{
				if( m_windowsLayer->messageBox( m_hWnd, L"This screensaver has no options you can set\nDo you want to launch game?", wprojectTitle.c_str(), MB_YESNO ) == IDNO )
				{
					return false;
				}
			}
			else
			{
				scriptInit += " screensaver";

				m_application->setFullscreenMode( true );
			}
		}

		SYSTEMTIME tm;
		GetLocalTime(&tm);
		LOGGER_INFO(m_serviceProvider)( "Date: %02d.%02d.%d, %02d:%02d:%02d"
			, tm.wDay
			, tm.wMonth
			, tm.wYear
			, tm.wHour
			, tm.wMinute
			, tm.wSecond 
			);

		OSVERSIONINFO os_ver;
		os_ver.dwOSVersionInfoSize = sizeof(os_ver);
		GetVersionEx(&os_ver);
		LOGGER_INFO(m_serviceProvider)( "OS: Windows %ld.%ld.%ld %S"
			, os_ver.dwMajorVersion
			, os_ver.dwMinorVersion
			, os_ver.dwBuildNumber 
			, os_ver.szCSDVersion
			);

		MEMORYSTATUS mem_st;
		GlobalMemoryStatus(&mem_st);
		LOGGER_INFO(m_serviceProvider)( "Memory: %ldK total, %ldK free, %ldK Page file total, %ldK Page file free"
			, mem_st.dwTotalPhys / 1024L
			, mem_st.dwAvailPhys / 1024L
			, mem_st.dwTotalPageFile / 1024L
			, mem_st.dwAvailPageFile / 1024L 
			);

		m_cursors[L"IDC_ARROW"] = LoadCursor( NULL, IDC_ARROW );
		m_cursors[L"IDC_UPARROW"] = LoadCursor( NULL, IDC_UPARROW );
		m_cursors[L"IDC_HAND"] = LoadCursor( NULL, IDC_HAND );
		m_cursors[L"IDC_HELP"] = LoadCursor( NULL, IDC_HELP );

		LOGGER_INFO(m_serviceProvider)( "Initializing Game data..." );

		if( m_application->initializeGame( scriptInit ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Application invalid initialize game"
				);

			return false;
		}

		bool alreadyRunning = CONFIG_VALUE(m_serviceProvider, "Game", "AlreadyRunning", true);

		if( alreadyRunning == true )
		{	
			m_alreadyRunningMonitor = new AlreadyRunningMonitor(m_serviceProvider);

			if( m_alreadyRunningMonitor->run( EARP_SETFOCUS, m_windowClassName, wprojectTitle ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("Application invalid running monitor"
					);

				return false;
			}
		}

		LOGGER_INFO(m_serviceProvider)( "Creating Render Window..." );

		bool fullscreen = m_application->getFullscreenMode();

		//if( m_application->isValidWindowMode() == false )
		//{
		//    fullscreen = true;
		//}

		if( m_windowsLayer->setProcessDPIAware() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Application not setup Process DPI Aware"
				);
		}

		Resolution windowResolution;
		m_application->calcWindowResolution( windowResolution );

		m_hWnd = this->createWindow( wprojectTitle, windowResolution, fullscreen );

		mt::vec2f point;
		this->calcCursorPosition( point );
		m_inputService->setCursorPosition( point );

		if( m_application->createRenderWindow( m_hWnd ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Application not create render window"
				);

			return false;
		}       

		m_vsync = m_application->getVSync();

		if( m_maxfps == false && m_vsync == false )
		{
			m_fpsMonitor = new FPSMonitor();
			m_fpsMonitor->initialize();

			m_fpsMonitor->setActive( true );
			m_fpsMonitor->setFrameTime( s_activeFrameTime );
		}
		
		if( m_maxfps == true && m_vsync == true )
		{
			m_application->setVSync( false );
		}

		HWND hWndFgnd = ::GetForegroundWindow();

		if( hWndFgnd != m_hWnd )
		{
			LOGGER_WARNING(m_serviceProvider)("Setup Foreground Window...");

			::ShowWindow(m_hWnd, SW_MINIMIZE);
			::ShowWindow(m_hWnd, SW_RESTORE);

			//SetWindowPos(m_hWnd,hWndFgnd,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
			//SetWindowPos(hWndFgnd,m_hWnd,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		}
		else
		{
			::ShowWindow( m_hWnd, SW_SHOW );
		}

		if( fullscreen == true )
		{
			Resolution desktopResolution;
			this->getDesktopResolution( desktopResolution );

			this->notifyWindowModeChanged( desktopResolution, true );
		}

		::SetForegroundWindow( m_hWnd );          // Slightly Higher Priority
		::SetFocus( m_hWnd );                     // Sets Keyboard Focus To The Window
		::UpdateWindow( m_hWnd );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::getMaxClientResolution( Resolution & _resolution ) const
	{
		RECT workArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

		RECT clientArea = workArea;
		::AdjustWindowRect( &clientArea, WS_OVERLAPPEDWINDOW, FALSE );
		uint32_t maxClientWidth = 2 * (workArea.right - workArea.left) - (clientArea.right - clientArea.left);
		uint32_t maxClientHeight = 2 * (workArea.bottom - workArea.top) - (clientArea.bottom - clientArea.top);

		_resolution.setWidth( maxClientWidth );
		_resolution.setHeight( maxClientHeight );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::loop()
	{
		m_winTimer->reset();

		try
		{
			while( m_running )
			{
				float frameTime = m_winTimer->getDeltaTime();

				//EXECUTION_STATE aState = ES_CONTINUOUS | ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED;
				//
				//if( m_windowsType == EWT_VISTA )
				//{
				//    aState = aState | ES_AWAYMODE_REQUIRED;
				//}

				//SetThreadExecutionState(aState);

				MSG  msg;
				while( m_windowsLayer->peekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
				{
					::TranslateMessage( &msg );

					m_windowsLayer->dispatchMessage( &msg );
				}

				bool updating = m_application->beginUpdate();

				if( updating == true )
				{
					m_application->tick( frameTime );
				}
				else
				{
					Sleep(100);
				}

				if( m_vsync == false && m_maxfps == false )
				{
					m_fpsMonitor->monitor();
				}

				if( m_application->isFocus() == true && m_active == true )
				{
					if( m_application->render() == true )
					{
						m_application->flush();
					}
				}     

				m_application->endUpdate();
			}
		}
		catch( const stdex::exception & ex )
		{
			LOGGER_CRITICAL(m_serviceProvider)("Win32ThreadIdentity stdex::exception '%s' file %d:%s"
				, ex.doc
				, ex.line
				, ex.file
				);
		}
		catch( const std::exception & ex )
		{
			LOGGER_CRITICAL(m_serviceProvider)("Win32ThreadIdentity std::exception '%s'"
				, ex.what()
				);
		}
		catch( ... )
		{			
			LOGGER_CRITICAL(m_serviceProvider)("Win32ThreadIdentity unsupported exception"
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::finalize()
	{
		m_mouseEvent.stop();
						
		if( m_hWnd != NULL )
		{
			m_windowsLayer->destroyWindow( m_hWnd );
			m_hWnd = NULL;
		}

		if( m_hInstance != NULL )
		{
			m_windowsLayer->unregisterClass( m_windowClassName.c_str(), m_hInstance );
		}	

		if( m_fpsMonitor != nullptr )
		{
			m_fpsMonitor->finalize();

			delete m_fpsMonitor;
			m_fpsMonitor = nullptr;
		}

		if( m_application != nullptr )
		{
			m_application->finalizeGame();
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

		if( m_pluginService != nullptr )
		{
			SERVICE_DESTROY( PluginService, m_pluginService );
			m_pluginService = nullptr;
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

		if( m_particleService2 != nullptr )
		{
			SERVICE_DESTROY( ParticleService2, m_particleService2 );
			m_particleService2 = nullptr;
		}

		SERVICE_DESTROY( PhysicSystem, m_physicSystem );
		m_physicSystem = nullptr;

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

		if( m_converterService != nullptr )
		{
			SERVICE_DESTROY( ConverterService, m_converterService );
			m_converterService = nullptr;
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

		if( m_alreadyRunningMonitor != nullptr )
		{
			m_alreadyRunningMonitor->stop();

			delete m_alreadyRunningMonitor;
			m_alreadyRunningMonitor = nullptr;
		}

		if( m_winTimer != nullptr )
		{
			delete m_winTimer;
			m_winTimer = nullptr;
		}

		if( m_windowsLayer != nullptr )
		{
			delete m_windowsLayer;
			m_windowsLayer = nullptr;
		}

		if( m_loggerConsole != nullptr )
		{
			if( m_logService != nullptr )
			{
				m_logService->unregisterLogger( m_loggerConsole );
			}

			delete m_loggerConsole;
			m_loggerConsole = nullptr;
		}

		if( m_loggerMessageBox != nullptr )
		{
			if( m_logService != nullptr )
			{
				m_logService->unregisterLogger( m_loggerMessageBox );
			}

			delete m_loggerMessageBox;
			m_loggerMessageBox = nullptr;
		}

		SERVICE_DESTROY( LogService, m_logService );
		m_logService = nullptr;

		SERVICE_DESTROY( ServiceProvider, m_serviceProvider );
		m_serviceProvider = nullptr;

		//::timeEndPeriod( 1 );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::stop()
	{
		m_running = false;
	}
	//////////////////////////////////////////////////////////////////////////
	static LRESULT CALLBACK s_wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		switch( uMsg )
		{
			//case WM_NCCREATE:
		case WM_CREATE:
			{
				LPCREATESTRUCTW createStruct = (LPCREATESTRUCTW)lParam;

				WinApplication * app = (WinApplication *)createStruct->lpCreateParams;

				::SetWindowLongPtr( hWnd, GWL_USERDATA, (LONG_PTR)app );

				return (LRESULT)NULL;
			}
			break;
		}

		LONG_PTR value = ::GetWindowLongPtr( hWnd, GWL_USERDATA );

		WinApplication * app = (WinApplication*)value;

		if( app == NULL )
		{
			LRESULT result = ::DefWindowProc( hWnd, uMsg, wParam, lParam );

			return result;
		}

		LRESULT app_result = app->wndProc( hWnd, uMsg, wParam, lParam );

		return app_result;
	}
	//////////////////////////////////////////////////////////////////////////
	LRESULT CALLBACK WinApplication::wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		if( m_hWnd != hWnd )
		{
			LRESULT result = ::DefWindowProc( hWnd, uMsg, wParam, lParam );

			return result;
		}
		//print "wndProc"
		//  if( uMsg != 0x200 && uMsg != 0x84 && uMsg != 0x20 )
		//  {
		//printf( "wndProc %x %x %x\n", uMsg, wParam, lParam );
		//  }

		switch( uMsg )
		{
			//case WM_QUERYENDSESSION:
			//	{
			//		printf("WM_QUERYENDSESSION");
			//	}break;
		case WM_ACTIVATE:
			{
				DWORD flagActive = LOWORD(wParam); // флажок активизации
				BOOL minimized = (BOOL) HIWORD(wParam); // флажок минимизации
				//HWND hwndPrevious = (HWND) lParam; // дескриптор окна 
				//bool active = (LOWORD(wParam) != WA_INACTIVE) && (HIWORD(wParam) == 0);

				bool active = (flagActive != WA_INACTIVE) && (minimized == FALSE);

				//printf("WM_ACTIVATE m_active %d %d %d\n"
				//                , active
				//                , minimized
				//                , hwndPrevious
				//                );

				//POINT cPos;
				//::GetCursorPos( &cPos );
				//::ScreenToClient( m_hWnd, &cPos );

				//mt::vec2f point;
				//point.x = static_cast<float>(cPos.x);
				//point.y = static_cast<float>(cPos.y);

				//m_application->setCursorPosition( point );
				//m_application->pushMouseMoveEvent( 0, 0, 0 );

				//printf("WinApplication::wndProc WM_ACTIVATE %d\n"
				//    , active
				//    );

				this->setActive( active );

				//::SetFocus( hWnd );

				return FALSE;
			}break;
		case WM_ACTIVATEAPP:
			{
				//BOOL active = (BOOL)wParam;

				//printf("WM_ACTIVATEAPP %d\n"
				//    , active
				//    );
			}break;
			//case WM_NCACTIVATE:
			//	{
			//              BOOL fActive = (BOOL)wParam;

			//              printf("WM_NCACTIVATE %d\n"
			//                  , fActive 
			//                  );
			//		//if( m_active )
			//		//{
			//		//	return FALSE;
			//		//}
			//              if( fActive == FALSE )
			//              {
			//                  return TRUE;
			//              }



			//              //return FALSE;
			//		//if( wParam == FALSE )
			//		//{
			//		//	return FALSE;
			//		//}
			//	}break;
		case WM_SETFOCUS:
			{
				//LOGGER_INFO(m_serviceProvider)("WM_SETFOCUS");
			}break;
		case WM_KILLFOCUS:
			{
				//LOGGER_INFO(m_serviceProvider)("WM_KILLFOCUS");
			}break;
		case WM_PAINT:
			{
				if( m_application->getFullscreenMode() == false )
				{
					m_application->paint();
				}
			}break;
		case WM_DISPLAYCHANGE:
			{
				LOGGER_WARNING(m_serviceProvider)("WinApplication::wndProc WM_DISPLAYCHANGE");

				//DWORD width = LOWORD(lParam); // флажок активизации
				//DWORD height = HIWORD(lParam); // флажок минимизации

				//Resolution desktopResolution;
				//desktopResolution.setWidth( width );
				//desktopResolution.setHeight( height );

				//bool fullscreenMode = m_application->getFullscreenMode();

				//this->notifyWindowModeChanged( desktopResolution, fullscreenMode );
				//m_menge->onWindowMovedOrResized();
			}break;
		case WM_SIZE:
			{
				if( wParam == SIZE_MAXIMIZED )
				{
					//m_application->setFullscreenMode( true );

					this->setActive( true );
				}
				else if( wParam == SIZE_MINIMIZED )
				{
					this->setActive( false );
				}
				else if( wParam == SIZE_RESTORED && m_application->getFullscreenMode() == true )
				{
					this->setActive( true );
				}
			}break;
		case WM_GETMINMAXINFO:
			{
				// Prevent the window from going smaller than some minimu size
				MINMAXINFO * info = (MINMAXINFO*)lParam;

				info->ptMinTrackSize.x = 100;
				info->ptMinTrackSize.y = 100;
			}break;
		case WM_CLOSE:
			{
				m_application->close();

				return FALSE;
			}break;
		case WM_SYSKEYDOWN:
			{
				unsigned int vkc = static_cast<unsigned int>( wParam );
				HKL  layout = ::GetKeyboardLayout(0);
				unsigned int vk = MapVirtualKeyEx( vkc, 0, layout );

				mt::vec2f point;
				this->calcCursorPosition(point);

				unsigned int tvk = translateVirtualKey_( vkc, vk );

				m_inputService->onKeyEvent( point, vkc, tvk, true );             
			}break;
		case WM_SYSKEYUP:
			{
				unsigned int vkc = static_cast<unsigned int>( wParam );
				HKL  layout = ::GetKeyboardLayout(0);
				unsigned int vk = MapVirtualKeyEx( vkc, 0, layout );

				mt::vec2f point;
				this->calcCursorPosition(point);

				unsigned int tvk = translateVirtualKey_( vkc, vk );

				m_inputService->onKeyEvent( point, vkc, tvk, false );
			}break;
		case WM_SYSCOMMAND:
			{
				switch( wParam )
				{
				case SC_CLOSE:
					{
						this->stop();
					}break;

				case SC_KEYMENU:
					{
						if( lParam == 13 )
						{					
							bool fullscreen = m_application->getFullscreenMode();
							m_application->setFullscreenMode( !fullscreen );
						}

						//return FALSE;
					}break;
				case SC_SCREENSAVE:
					{
						//Disable Screensave
						return TRUE;
					}break;
				case SC_MONITORPOWER:
					{
						return TRUE;
					}break;
				}            
			}break;
		case WM_SETCURSOR:
			{
				if( m_application->isFocus() && LOWORD(lParam) == HTCLIENT && m_cursorMode == false )
				{
					::SetCursor(NULL);
				}
				else 
				{
					if (m_cursor == NULL)
					{
						m_cursor = LoadCursor(NULL, IDC_ARROW);						
					}

					::SetCursor( m_cursor );

					//if( m_clipingCursor != FALSE )
					//{
					//	::ClipCursor( &m_clipingCursorRect );
					//}
				}

				return FALSE;
			}break;
		case WM_DESTROY: 
			::PostQuitMessage(0);

			return 0;
		}

		BOOL input_result;
		if( this->wndProcInput( hWnd, uMsg, wParam, lParam, input_result ) == true )
		{
			return input_result;
		}

		LRESULT result = m_windowsLayer->defWindowProc( hWnd, uMsg, wParam, lParam );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::wndProcInput( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & _result )
	{
		(void)hWnd;

		bool handle = false;

		switch( uMsg )
		{
		case WM_TIMER:
			{
				if( wParam == UTIMER_MOUSE_EVENT )
				{
					m_mouseEvent.verify( hWnd );
				}

				handle = true;
				_result = FALSE;
			}break;
		case UWM_MOUSE_LEAVE:
			{
				if( m_cursorInArea == true )
				{
					m_cursorInArea = false;
					InvalidateRect(hWnd, NULL, FALSE);
					UpdateWindow(hWnd);

					mt::vec2f point;
					this->calcCursorPosition( point );

					m_inputService->onMouseLeave( 0, point );

					if( (GetKeyState( VK_LBUTTON ) & 0x8000) != 0 )
					{
						m_clickOutArea = true;
					}
				}

				handle = true;
				_result = FALSE;
			}break;
		case WM_MOUSEMOVE:
			{
				//::SetFocus( m_hWnd );

				mt::vec2f point;
				this->calcCursorPosition( point );

				m_mouseEvent.notify( hWnd );

				if( m_cursorInArea == false )
				{
					m_cursorInArea = true;

					InvalidateRect(hWnd, NULL, FALSE);
					UpdateWindow(hWnd);

					m_inputService->onMouseEnter( 0, point );
				}

				if( m_clickOutArea == true ) 
				{
					m_clickOutArea = false;

					if( (GetKeyState( VK_LBUTTON ) & 0x8000) == 0 )
					{
						m_inputService->onMouseButtonEvent( 0, point, 0, false );
					}                    
				}

				int x = (int)(short)LOWORD(lParam);
				int y = (int)(short)HIWORD(lParam);
				int dx = x - m_lastMouseX;
				int dy = y - m_lastMouseY;

				if( dx == 0 && dy == 0 )
				{
					break;
				}

				float fdx = (float)dx;
				float fdy = (float)dy;

				//const Resolution & contentResolution = m_application->getContentResolution();
				//mt::vec2f resolutionScale = contentResolution.getScale( m_windowResolution );

				//float fdx_scale = fdx * resolutionScale.x;
				//float fdy_scale = fdy * resolutionScale.y;

				RECT rect;
				if( GetClientRect( m_hWnd, &rect ) == FALSE )
				{
					return false;
				}

				float width = static_cast<float>(rect.right - rect.left);
				float height = static_cast<float>(rect.bottom - rect.top);

				fdx /= width;
				fdy /= height;

				m_inputService->onMouseMove( 0, point, fdx, fdy );

				m_lastMouseX = x;
				m_lastMouseY = y;

				handle = true;
				_result = FALSE;
			}break;
		case WM_MOUSEWHEEL:
			{
				int zDelta = (int)(short)( HIWORD(wParam) );

				mt::vec2f point;
				this->calcCursorPosition( point );				

				int wheel = zDelta / WHEEL_DELTA;

				m_inputService->onMouseWheel( 0, point, wheel );

				handle = true;
				_result = FALSE;
			}break;
		case WM_RBUTTONDBLCLK:
		case WM_LBUTTONDBLCLK:
			{
				m_isDoubleClick = true;

				handle = true;
				_result = FALSE;
			}
			break;
		case WM_LBUTTONDOWN:
			{
				mt::vec2f point;
				this->calcCursorPosition( point );

				m_inputService->onMouseButtonEvent( 0, point, 0, true );				

				handle = true;
				_result = FALSE;
			}
			break;
		case WM_LBUTTONUP:
			{
				if( m_isDoubleClick == false )
				{
					mt::vec2f point;
					this->calcCursorPosition(point);

					m_inputService->onMouseButtonEvent( 0, point, 0, false );					
				}

				m_isDoubleClick = false;

				handle = true;
				_result = FALSE;
			}break;
		case WM_RBUTTONDOWN:
			{
				//::SetFocus(m_hWnd);

				mt::vec2f point;
				this->calcCursorPosition(point);

				m_inputService->onMouseButtonEvent( 0, point, 1, true );				

				handle = true;
				_result = FALSE;
			}break;
		case WM_RBUTTONUP:
			{
				if( m_isDoubleClick == false )
				{
					mt::vec2f point;
					this->calcCursorPosition(point);

					m_inputService->onMouseButtonEvent( 0, point, 1, false );
				}

				m_isDoubleClick = false;

				handle = true;
				_result = FALSE;
			}break;
		case WM_MBUTTONDOWN:
			{
				//::SetFocus(m_hWnd);

				mt::vec2f point;
				this->calcCursorPosition(point);

				m_inputService->onMouseButtonEvent( 0, point, 2, true );	

				handle = true;
				_result = FALSE;
			}break;
		case WM_MBUTTONUP:
			{
				mt::vec2f point;
				this->calcCursorPosition(point);

				m_inputService->onMouseButtonEvent( 0, point, 2, false );	

				handle = true;
				_result = FALSE;
			}break;
		case WM_KEYDOWN:
			{
				UINT vkc = static_cast<UINT>( wParam );
				HKL  layout = ::GetKeyboardLayout(0);
				UINT vk = MapVirtualKeyEx( vkc, 0, layout );

				mt::vec2f point;
				this->calcCursorPosition(point);

				UINT tvk = this->translateVirtualKey_( vkc, vk );

				m_inputService->onKeyEvent( point, vkc, tvk, true );				

				handle = true;
				_result = FALSE;
			}break;
		case WM_KEYUP:
			{
				UINT vkc = static_cast<UINT>( wParam );
				HKL  layout = ::GetKeyboardLayout(0);
				unsigned int vk = MapVirtualKeyEx( vkc, 0, layout );

				mt::vec2f point;
				this->calcCursorPosition(point);

				UINT tvk = this->translateVirtualKey_( vkc, vk );

				m_inputService->onKeyEvent( point, vkc, tvk, false );		

				handle = true;
				_result = FALSE;
			}break;
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	HWND WinApplication::createWindow( const Menge::WString & _projectTitle, const Resolution & _resolution, bool _fullscreen )
	{
		m_windowResolution = _resolution;

		HBRUSH black_brush = (HBRUSH)GetStockObject(BLACK_BRUSH);

		// Register the window class		
		ATOM result = m_windowsLayer->registerClass( 
			s_wndProc, 
			0, 
			0, 
			m_hInstance, 
			IDI_MENGE, 
			black_brush, 
			m_windowClassName.c_str()
			);

		if( result == FALSE )
		{
			LOGGER_ERROR(m_serviceProvider)("Can't register window class");

			return NULL;
		}

		DWORD dwStyle = this->getWindowStyle( _fullscreen );
		dwStyle &= ~WS_VISIBLE;

		RECT rc = this->getWindowsRect( m_windowResolution, _fullscreen );

		DWORD exStyle = _fullscreen ? WS_EX_TOPMOST : 0;
		//DWORD exStyle = 0;

		HWND hwnd = m_windowsLayer->createWindowEx( exStyle, m_windowClassName.c_str(), _projectTitle.c_str(), dwStyle
			, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top
			, NULL, NULL, m_hInstance, (LPVOID)this );

		return hwnd; 
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
	{
		LOGGER_WARNING(m_serviceProvider)("WinApplication::notifyWindowModeChanged %d:%d %d"
			, _resolution.getWidth()
			, _resolution.getHeight()
			, _fullscreen
			);

		m_windowResolution = _resolution;

		DWORD dwStyle = this->getWindowStyle( _fullscreen );

		RECT rc = this->getWindowsRect( m_windowResolution, _fullscreen );

		DWORD dwExStyle = m_windowsLayer->getWindowLong( m_hWnd, GWL_EXSTYLE );

		if( _fullscreen == false )
		{
			// When switching back to windowed mode, need to reset window size 
			// after device has been restored

			m_windowsLayer->setWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyle & (~WS_EX_TOPMOST) );
			m_windowsLayer->setWindowLong( m_hWnd, GWL_STYLE, dwStyle );

			::SetWindowPos(
				m_hWnd
				, HWND_NOTOPMOST
				, rc.left
				, rc.top
				, rc.right - rc.left
				, rc.bottom - rc.top
				, SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOACTIVATE
				);
		}
		else
		{
			m_windowsLayer->setWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyle | WS_EX_TOPMOST );
			m_windowsLayer->setWindowLong( m_hWnd, GWL_STYLE, dwStyle );

			::SetWindowPos( 
				m_hWnd
				, HWND_TOPMOST
				, rc.left
				, rc.top
				, rc.right - rc.left
				, rc.bottom - rc.top
				//, SWP_FRAMECHANGED //| SWP_NOZORDER | SWP_NOACTIVATE  | SWP_FRAMECHANGED
				, SWP_NOACTIVATE
				);


			//::SetWindowPos(
			//    m_hWnd
			//    , 0
			//    , 0
			//    , 0
			//    , 0
			//    , 0
			//    , SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
			//    );
		}

		//::UpdateWindow( m_hWnd );
		//::ShowWindow( m_hWnd, SW_SHOW );
		//      ::SetFocus( m_hWnd );
	}
	//////////////////////////////////////////////////////////////////////////
	DWORD WinApplication::getWindowStyle( bool _fullsreen ) const
	{
		DWORD dwStyle = WS_POPUP;

		if( _fullsreen == false )
		{
			dwStyle |= WS_CAPTION | WS_VISIBLE;

			bool hasWindowPanel = true;
			//m_application->getHasWindowPanel();

			if( hasWindowPanel == true )
			{
				dwStyle |= WS_SYSMENU | WS_MINIMIZEBOX;
			}

			bool allowMaximize = m_application->getAllowFullscreenSwitchShortcut();

			if( allowMaximize == true )
			{
				dwStyle |= WS_MAXIMIZEBOX;
			}
		}
		else
		{
			dwStyle |= WS_VISIBLE;
		}

		return dwStyle;
	}
	//////////////////////////////////////////////////////////////////////////
	RECT WinApplication::getWindowsRect( const Resolution & _resolution, bool _fullsreen ) const
	{
		RECT rc;
		::SetRect(&rc, 0, 0, (int)_resolution.getWidth(), (int)_resolution.getHeight());

		if( _fullsreen == false )
		{
			DWORD dwStyle = this->getWindowStyle( _fullsreen );
			::AdjustWindowRect( &rc, dwStyle, FALSE );

			RECT workArea;
			::SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

			LONG width = rc.right - rc.left;
			LONG height = rc.bottom - rc.top;

			rc.left = (workArea.left + workArea.right - width) / 2;
			rc.top = (workArea.top + workArea.bottom - height) / 2;

			rc.right = rc.left + width;
			rc.bottom = rc.top + height;
		}

		return rc;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::setActive( bool _active )
	{
		if( m_active == _active )
		{
			return;
		}

		m_active = _active;

		//if( m_clipingCursor != FALSE )
		//{
		//	::ClipCursor( (m_active)?(&m_clipingCursorRect):NULL );
		//}

		if( m_active == true )
		{
			if (m_cursor == NULL)
			{
				m_cursor = LoadCursor(NULL, IDC_ARROW);						
			}

			::SetCursor( m_cursor );
		}
		else
		{
			::SetCursor( NULL );
		}

		if( m_fpsMonitor )
		{
			if( m_active == true )
			{
				m_fpsMonitor->setFrameTime( s_activeFrameTime );
			}
			else
			{
				m_fpsMonitor->setFrameTime( s_inactiveFrameTime );
			}
		}

		mt::vec2f point;
		this->calcCursorPosition(point);

		m_inputService->onMousePosition( 0, point );

		if( m_nopause == false )
		{
			m_application->setFocus( m_active, point );
			m_inputService->onFocus( m_active );

			bool turnSound = m_active;
			m_application->turnSound( turnSound );
		}
		else
		{
			m_application->setFocus( true, point );
			m_inputService->onFocus( true );

			m_application->turnSound( true );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::calcCursorPosition( mt::vec2f & _point ) const
	{
		POINT cPos;
		if( ::GetCursorPos( &cPos ) == FALSE )
		{
			return false;
		}

		if( ::ScreenToClient( m_hWnd, &cPos ) == FALSE )
		{
			return false;
		}

		float x = static_cast<float>(cPos.x);
		float y = static_cast<float>(cPos.y);

		RECT rect;
		if( GetClientRect( m_hWnd, &rect ) == FALSE )
		{
			return false;
		}

		float width = static_cast<float>(rect.right - rect.left);
		float height = static_cast<float>(rect.bottom - rect.top);

		_point.x = x / width;
		_point.y = y / height;

		//_point.x = x;
		//_point.y = y;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::createDirectoryUserPicture( const WString & _path, const WString & _file, const void * _data, size_t _size )
	{
		bool successful = WINDOWSLAYER_SERVICE(m_serviceProvider)
			->createDirectoryUserPicture(_path, _file, _data, _size);

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::createDirectoryUserMusic( const WString & _path, const WString & _file, const void * _data, size_t _size )
	{
		bool successful = WINDOWSLAYER_SERVICE(m_serviceProvider)
			->createDirectoryUserMusic(_path, _file, _data, _size);

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::getDesktopResolution( Resolution & _resolution ) const
	{
		int cxscreen = ::GetSystemMetrics( SM_CXSCREEN );
		int cyscreen = ::GetSystemMetrics( SM_CYSCREEN );

		_resolution.setWidth( cxscreen );
		_resolution.setHeight( cyscreen );
	}
	//////////////////////////////////////////////////////////////////////////
	const String & WinApplication::getCurrentPath() const
	{
		static String path;
		if( Helper::unicodeToUtf8( m_serviceProvider, m_currentPath, path ) == false )
		{
			return path;
		}

		return path;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::minimizeWindow()
	{
		::ShowWindow( m_hWnd, SW_MINIMIZE );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyVsyncChanged( bool _vsync )
	{
		m_vsync = _vsync;

		if( m_fpsMonitor )
		{
			m_fpsMonitor->setActive( m_vsync == false );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::setHandleMouse( bool _handle )
	{
		(void)_handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::setCursorPosition( const mt::vec2f & _pos )
	{        
		POINT cPos = { (int)_pos.x, (int)_pos.y };

		::ClientToScreen( m_hWnd, &cPos );

		::SetCursorPos( cPos.x, cPos.y );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::showKeyboard()
	{
		//Empty - for iPad
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::hideKeyboard()
	{
		//Empty - for iPad
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyCursorModeChanged( bool _mode )
	{
		m_cursorMode = _mode;

		if( m_cursor == NULL )
		{
			m_cursor = ::LoadCursor( NULL, IDC_ARROW );
		}

		m_cursor = m_cursorMode ? m_cursor : NULL;

		::SetCursor( m_cursor );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyCursorClipping( const Viewport & _viewport )
	{
		(void)_viewport;

		//POINT p1 = { static_cast<LONG>( _viewport.begin.x ),
		//	static_cast<LONG>( _viewport.begin.y ) };

		//POINT p2 = { static_cast<LONG>( _viewport.end.x ),
		//	static_cast<LONG>( _viewport.end.y ) };

		//::ClientToScreen( m_hWnd, &p1 );
		//::ClientToScreen( m_hWnd, &p2 );

		//m_clipingCursorRect.left = p1.x;
		//m_clipingCursorRect.top = p1.y;
		//m_clipingCursorRect.right = p2.x;
		//m_clipingCursorRect.bottom = p2.y;

		//m_clipingCursor = ::ClipCursor( &m_clipingCursorRect );	// Bound cursor
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyCursorUnClipping()
	{
		//m_clipingCursor = FALSE;

		//::ClipCursor( NULL );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyCursorIconSetup( const FilePath & _name, void * _buffer, size_t _size )
	{
		WString unicode_name;        
		if( Helper::utf8ToUnicode( m_serviceProvider, _name, unicode_name ) == false )
		{
			return;
		}

		TMapCursors::iterator it_found = m_cursors.find( unicode_name );

		if( it_found == m_cursors.end() )
		{
			WString icoDir;

			icoDir += m_userPath;
			icoDir += L"IconCache";

			m_windowsLayer->createDirectory( icoDir.c_str() );

			WString icoFile;

			icoFile += icoDir;
			icoFile += MENGE_FOLDER_DELIM;
			icoFile += unicode_name;

			WString::size_type pos1 = icoFile.find_last_of( L'/' );
			WString::size_type pos2 = icoFile.find_last_of( L'\\' );

			WString::size_type pos = (std::max)(pos1, pos2);

			WString icoDir2 = icoFile.substr( 0, pos );

			m_windowsLayer->createDirectory( icoDir2.c_str() );

			FILE * file = _wfopen( icoFile.c_str(), L"wb" );

			if( file == nullptr )
			{
				LOGGER_ERROR(m_serviceProvider)("WinApplication::notifyCursorIconSetup can't create cursor cach file %ls"
					, icoFile.c_str()
					);

				return;
			}

			fwrite( _buffer, _size, 1, file );
			fclose( file );

			//HCURSOR cursor = LoadCursorFromFile( _fileName.c_str() );
			HCURSOR cursor = ::LoadCursorFromFile( icoFile.c_str() );

			DWORD errCode = ::GetLastError();

			if( errCode != 0 )
			{
				LOGGER_ERROR(m_serviceProvider)("WinApplication::notifyCursorIconSetup errCode %d"
					, errCode 
					);

				return;
			}

			it_found = m_cursors.insert( std::make_pair( unicode_name, cursor ) ).first;
		}

		m_cursor = it_found->second;

		::SetCursor( m_cursor );
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int WinApplication::translateVirtualKey_( unsigned int _vkc, unsigned int _vk )
	{
		if( _vk == 0 )
		{
			return 0;
		}

		BYTE keyState[256];
		if( ::GetKeyboardState( keyState ) == 0 )
		{
			return 0;
		}

		HKL layout = ::GetKeyboardLayout(0);

		WCHAR wide[3];
		int res = ::ToUnicodeEx( _vkc, _vk, keyState, wide, 3, 0, layout );

		if( res == -1 )
		{
			m_deadKey = wide[0];
		}
		else if( res > 0 )
		{
			return wide[0];
		}

		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::isSaverRunning() const
	{ 		
		WChar fileName[MAX_PATH];
		if( m_windowsLayer->getModuleFileName( NULL, fileName, MAX_PATH ) == false )
		{
			return false;
		}

		size_t fn_len = wcslen( fileName );

		if( fn_len < 4 )
		{
			return false;
		}

		WChar * extention = fileName + (fn_len - 4);

		if( wcscmp( extention, L".scr" ) == 0 )
		{
			return true;
		}

		return false;
	}
	////////////////////////////////////////////////////////////////////////////
	//void WinApplication::setAsScreensaver( bool _set )
	//{
	//	if( _set == true )
	//	{
	//		WString screensaverName = m_application->getScreensaverName();

	//		FilePath fullModuleName;
	//		WindowsLayer::getModuleFileName(NULL, fullModuleName);

	//		size_t separatorPos = fullModuleName.find_last_of(MENGE_FOLDER_DELIM);
	//		FilePath binFolderPath = fullModuleName.substr(0, separatorPos);
	//		FilePath fullScreensaverPath = binFolderPath + MENGE_FOLDER_DELIM + screensaverName;

	//		FilePath fullScreensaverPathShort;
	//		WindowsLayer::getShortPathName( fullScreensaverPath, fullScreensaverPathShort );

	//		WindowsLayer::setRegistryValue( HKEY_CURRENT_USER
	//			, L"Control Panel\\Desktop"
	//			, L"SCRNSAVE.EXE"
	//			, REG_SZ
	//			, reinterpret_cast<const BYTE*>( fullScreensaverPathShort.c_str() )
	//			, fullScreensaverPathShort.length() + 1 
	//			);
	//	}
	//	else
	//	{
	//		WindowsLayer::deleteRegistryValue( HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"SCRNSAVE.EXE" );
	//	}		
	//}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::isDevelopmentMode() const
	{
		return m_developmentMode;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::onEvent( const ConstString & _event, const TMapParams & _params )
	{
		(void)_event;
		(void)_params;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::openUrlInDefaultBrowser( const WString & _url )
	{
		//size_t cmd_path_size = 0;
		//wchar_t cmd_path [MAX_PATH];
		wchar_t * cmd_path = _wgetenv( L"COMSPEC" );

		WString params = L"/c start " + _url;

		STARTUPINFO startup_info;
		memset( &startup_info, 0, sizeof(startup_info) );
		startup_info.cb = sizeof (startup_info);
		startup_info.wShowWindow = SW_HIDE;
		startup_info.dwFlags |= STARTF_USESHOWWINDOW;

		PROCESS_INFORMATION process_info;
		memset( &process_info, 0, sizeof(process_info) );

		BOOL result = ::CreateProcess (
			cmd_path,          // path
			const_cast<LPWSTR>(params.c_str()), // command line
			NULL,            // process attributes
			NULL,            // thread attributes
			FALSE,            // inherit handles
			NORMAL_PRIORITY_CLASS,    // creation flags
			NULL,            // environment
			NULL,            // current directory
			& startup_info,        // startup info structure
			& process_info        // process info structure
			);

		LOGGER_WARNING(m_serviceProvider)("WinApplication::openUrlInDefaultBrowser %ls %d"
			, _url.c_str()
			, result
			);

		if( result == FALSE )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
