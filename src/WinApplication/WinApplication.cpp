#	include "Config/Config.h"
#	include "Config/Stringstream.h"

#	include "WinApplication.h"

#	include "Interface/OptionsInterface.h"
#	include "Interface/LoggerInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/InputSystemInterface.h"
#	include "Interface/ConfigInterface.h"
#	include "Interface/PackageInterface.h"

#	include "WindowsLayer/VistaWindowsLayer.h"

#	include "Factory/FactorableUnique.h"
#	include "Factory/FactoryDefault.h"

#	include "Core/Date.h"

#	include "Logger/Logger.h"

#	include <cstdio>
#	include <clocale>

#   include <errno.h>

#	include "ConsoleLogger.h"
#	include "MessageBoxLogger.h"

#	include "CriticalErrorsMonitor.h"

#	include "Core/FileLogger.h"
#	include "Core/IniUtil.h"

#	include "resource.h"

#	include <ctime>
#	include <algorithm>
#	include <functional>

#	include <sstream>

#   include <WinBase.h>
#   include <Psapi.h>
#	include <tlhelp32.h>


SERVICE_PROVIDER_EXTERN( ServiceProvider );

SERVICE_EXTERN( OptionsService );
SERVICE_EXTERN( OptionsSystem );
SERVICE_EXTERN( StringizeService );
SERVICE_EXTERN( LoggerService );
SERVICE_EXTERN( WindowsLayer );
SERVICE_EXTERN( Platform );
SERVICE_EXTERN( NotificationService );
SERVICE_EXTERN( UnicodeService );
SERVICE_EXTERN( UnicodeSystem );
SERVICE_EXTERN( FileService );
SERVICE_EXTERN( ConfigService );
SERVICE_EXTERN( ArchiveService );
SERVICE_EXTERN( ThreadService );
SERVICE_EXTERN( ThreadSystem );
SERVICE_EXTERN( ParticleService );
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
SERVICE_EXTERN( TimerService );
SERVICE_EXTERN( TimerSystem );
SERVICE_EXTERN( PluginSystem );
SERVICE_EXTERN( PluginService );
SERVICE_EXTERN( Application );

PLUGIN_EXPORT( MengeImageCodec );
PLUGIN_EXPORT( MengeSoundCodec );
PLUGIN_EXPORT( MengeVideoCodec );
PLUGIN_EXPORT( MengeAmplifier );
PLUGIN_EXPORT( MengeZip );
PLUGIN_EXPORT( MengeLZ4 );
PLUGIN_EXPORT( MengeSpine );
PLUGIN_EXPORT( MengeOggVorbis );
PLUGIN_EXPORT( MengeWin32FileGroup );
PLUGIN_EXPORT( Box2D );

PLUGIN_EXPORT( PathFinder );

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	WinApplication::WinApplication() 
		: m_serviceProvider(nullptr)
		, m_loggerConsole(nullptr)
		, m_loggerMessageBox(nullptr)	
		, m_fileLog(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	WinApplication::~WinApplication()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initializeArchiveService_()
	{
		{
			LOGGER_INFO(m_serviceProvider)("Initialize Zip...");
			PLUGIN_CREATE( m_serviceProvider, MengeZip );
		}

		{
			LOGGER_INFO(m_serviceProvider)("Initialize LZ4...");
			PLUGIN_CREATE( m_serviceProvider, MengeLZ4 );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::getApplicationPath_( const char * _section, const char * _key, ConstString & _path )
	{
		FilePath applicationPath = STRINGIZE_STRING_LOCAL( m_serviceProvider, "application.ini" );

		InputStreamInterfacePtr applicationInputStream = FILE_SERVICE(m_serviceProvider)
			->openInputFile( ConstString::none(), applicationPath, false );

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

		const ConstString & platformName = PLATFORM_SERVICE( m_serviceProvider )
			->getPlatformName();

		CONFIG_SERVICE( m_serviceProvider )
			->setPlatformName( platformName );

		ConstString gameIniPath;
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

		{
			LOGGER_INFO(m_serviceProvider)("Initialize Win32 file group...");
			PLUGIN_CREATE( m_serviceProvider, MengeWin32FileGroup );
		}

#	ifndef _MSC_VER
			WINDOWSLAYER_SERVICE(m_serviceProvider)
				->setModuleCurrentDirectory();
#	endif

		WChar currentPath[MENGINE_MAX_PATH];

		size_t currentPathLen = WINDOWSLAYER_SERVICE( m_serviceProvider )
			->getCurrentDirectory( currentPath, MENGINE_MAX_PATH );
		
		if( currentPathLen == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::setupFileService: failed to get current directory"
				);

			return false;
		}

		String utf8_currentPath;
		if( Helper::unicodeToUtf8Size( m_serviceProvider, currentPath, currentPathLen, utf8_currentPath ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::setupFileService: failed to convert %ls to utf8"
				, currentPath
				);

			return false;
		}

		LOGGER_WARNING(m_serviceProvider)("Current Path %ls"
			, currentPath
			);
		
		// mount root		
		if( FILE_SERVICE(m_serviceProvider)->mountFileGroup( ConstString::none(), Helper::stringizeString( m_serviceProvider, utf8_currentPath ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "dir" ) ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::setupFileService: failed to mount application directory %ls"
				, currentPath
				);

			return false;
		}

#	ifndef MENGINE_MASTER_RELEASE
		// mount root		
		if( FILE_SERVICE( m_serviceProvider )->mountFileGroup( STRINGIZE_STRING_LOCAL( m_serviceProvider, "dev" ), ConstString::none(), STRINGIZE_STRING_LOCAL( m_serviceProvider, "dir" ) ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication::setupFileService: failed to mount dev directory %ls"
				, currentPath
				);

			return false;
		}
#   endif

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::makeUserPath_( WString & _wstring ) const
	{ 
		_wstring.clear();

		bool developmentMode = HAS_OPTION( m_serviceProvider, "dev" );
		bool roamingMode = HAS_OPTION( m_serviceProvider, "roaming" );
		bool noroamingMode = HAS_OPTION( m_serviceProvider, "noroaming" );
		
		if( developmentMode == true && roamingMode == false || noroamingMode == true )
		{
			WChar currentPath[MENGINE_MAX_PATH];
			size_t currentPathLen = WINDOWSLAYER_SERVICE(m_serviceProvider)
				->getCurrentDirectory( currentPath, MENGINE_MAX_PATH );
			
			if( currentPathLen == 0 )
			{
				LOGGER_ERROR( m_serviceProvider )("WinApplication::makeUserPath_: failed to get current directory"
					);

				return false;
			}

			_wstring += currentPath;
			_wstring += L"User";
			_wstring += L'\\';
		}
		else	// create user directory in ~/Local Settings/Application Data/<uUserPath>/
		{
			WChar buffer[MENGINE_MAX_PATH];
			LPITEMIDLIST itemIDList;

			HRESULT hr = SHGetSpecialFolderLocation( NULL,
				CSIDL_APPDATA | CSIDL_FLAG_CREATE, &itemIDList );

			if( hr != S_OK )
			{
				WString msg;

				if( WINDOWSLAYER_SERVICE( m_serviceProvider )->makeFormatMessage( hr, msg ) == false )
				{
					LOGGER_ERROR( m_serviceProvider )("SHGetSpecialFolderLocation invalid %d"
						, hr
						);
				}
				else
				{
					LOGGER_ERROR( m_serviceProvider )("SHGetSpecialFolderLocation invalid %ls '%d'"
						, msg.c_str()
						, hr
						);
				}

				return false;
			}

			BOOL result = SHGetPathFromIDListW( itemIDList, buffer );

			if( result == FALSE )
			{
				LOGGER_ERROR( m_serviceProvider )("SHGetPathFromIDListW invalid"
					);

				return false;
			}

			CoTaskMemFree( itemIDList );

			_wstring = buffer;
			_wstring += L'\\';
			_wstring += CONFIG_VALUE( m_serviceProvider, "Project", "Company", L"NONAME" );
			_wstring += L'\\';
			_wstring += CONFIG_VALUE( m_serviceProvider, "Project", "Name", L"UNKNOWN" );
			_wstring += L'\\';
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initializeUserDirectory_()
	{
		//m_tempPath.clear();
		WString userPath;
		this->makeUserPath_( userPath );

		String utf8_userPath;
		if( Helper::unicodeToUtf8( m_serviceProvider, userPath, utf8_userPath ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication: failed user directory %ls convert to ut8f"
				, userPath.c_str()
				);

			return false;
		}

		// mount user directory
		if( FILE_SERVICE(m_serviceProvider)->mountFileGroup( STRINGIZE_STRING_LOCAL( m_serviceProvider, "user" ), Helper::stringizeString( m_serviceProvider, utf8_userPath ), STRINGIZE_STRING_LOCAL( m_serviceProvider, "dir" ) ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("WinApplication: failed to mount user directory %ls"
				, userPath.c_str()
				);

			return false;
		}

		if( FILE_SERVICE( m_serviceProvider )->existDirectory( STRINGIZE_STRING_LOCAL( m_serviceProvider, "user" ), ConstString::none() ) == false )
		{
			FILE_SERVICE( m_serviceProvider )->createDirectory( STRINGIZE_STRING_LOCAL( m_serviceProvider, "user" ), ConstString::none() );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initializeLogFile_()
	{
		bool nologs = HAS_OPTION( m_serviceProvider, "nologs" );
		
		if( nologs == true )
		{
			return true;
		}

		WString date;
		Helper::makeDateTime( date );

		WString unicode_logFilename;
		unicode_logFilename += L"Game";

		bool developmentMode = HAS_OPTION( m_serviceProvider, "dev" );
		bool roamingMode = HAS_OPTION( m_serviceProvider, "roaming" );
		bool noroamingMode = HAS_OPTION( m_serviceProvider, "noroaming" );

		if( developmentMode == true && roamingMode == false || noroamingMode == false )
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

		OutputStreamInterfacePtr fileLogInterface = FILE_SERVICE( m_serviceProvider )
			->openOutputFile( STRINGIZE_STRING_LOCAL( m_serviceProvider, "user" ), logFilename );

		if( fileLogInterface != nullptr )
		{
			m_fileLog = new FileLogger();

			LOGGER_SERVICE(m_serviceProvider)
				->registerLogger( m_fileLog );

			LOGGER_INFO(m_serviceProvider)("WinApplication: Starting log to %s"
				, logFilename.c_str()
				);
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initializeLogEngine_()
	{
		bool nologs = HAS_OPTION( m_serviceProvider, "nologs" );

		bool console = HAS_OPTION( m_serviceProvider, "console" );

		if( console == true && nologs == false )
		{
			m_loggerConsole = new ConsoleLogger();

			LOGGER_SERVICE( m_serviceProvider )
				->registerLogger( m_loggerConsole );
		}
		
		if( nologs == false )
		{
			m_loggerMessageBox = new MessageBoxLogger();

			m_loggerMessageBox->setVerboseLevel( LM_CRITICAL );

			LOGGER_SERVICE( m_serviceProvider )
				->registerLogger( m_loggerMessageBox );
		}
				
		EMessageLevel m_logLevel;

		bool developmentMode = HAS_OPTION( m_serviceProvider, "dev" );
		bool roamingMode = HAS_OPTION( m_serviceProvider, "roaming" );
		bool noroamingMode = HAS_OPTION( m_serviceProvider, "noroaming" );

		if( developmentMode == true && roamingMode == false || noroamingMode == true )
		{
			m_logLevel = LM_LOG;
		}
		else
		{
			m_logLevel = LM_ERROR;
		}

		if( HAS_OPTION( m_serviceProvider, "log:0" ) == true )
		{
			m_logLevel = LM_INFO;
		}
		else if( HAS_OPTION( m_serviceProvider, "log:1" ) == true )
		{
			m_logLevel = LM_LOG;
		}
		else if( HAS_OPTION( m_serviceProvider, "log:2" ) == true )
		{
			m_logLevel = LM_WARNING;
		}
		else if( HAS_OPTION( m_serviceProvider, "log:3" ) == true )
		{
			m_logLevel = LM_ERROR;
		}
		else if( HAS_OPTION( m_serviceProvider, "log:4" ) == true )
		{
			m_logLevel = LM_CRITICAL;
		}
		else if( HAS_OPTION( m_serviceProvider, "log:5" ) == true )
		{
			m_logLevel = LM_FATAL;
		}

		LOGGER_SERVICE( m_serviceProvider )
			->setVerboseLevel( m_logLevel );

		uint32_t verboseFlag = 0;

		bool profiler = HAS_OPTION( m_serviceProvider, "profiler" );

		if( profiler == true )
		{
			verboseFlag |= 0x00000001;
		}

		LOGGER_SERVICE( m_serviceProvider )
			->setVerboseFlag( verboseFlag );

		if( HAS_OPTION( m_serviceProvider, "verbose" ) == true )
		{
			LOGGER_SERVICE( m_serviceProvider )
				->setVerboseLevel( LM_MAX );

			LOGGER_INFO(m_serviceProvider)( "Verbose logging mode enabled" );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initialize()
	{		
		setlocale( LC_ALL, "C" );
		//::timeBeginPeriod( 1 );

		ServiceProviderInterface * serviceProvider;
		SERVICE_PROVIDER_CREATE( ServiceProvider, &serviceProvider );

		m_serviceProvider = serviceProvider;

		SERVICE_CREATE( m_serviceProvider, OptionsSystem );
		SERVICE_CREATE( m_serviceProvider, OptionsService );

		SERVICE_CREATE( m_serviceProvider, StringizeService );
		SERVICE_CREATE( m_serviceProvider, LoggerService );

		if( this->initializeLogEngine_() == false )
		{
			return false;
		}

		SERVICE_CREATE( m_serviceProvider, WindowsLayer );
		SERVICE_CREATE( m_serviceProvider, Platform );

		SERVICE_CREATE( m_serviceProvider, UnicodeSystem );
		SERVICE_CREATE( m_serviceProvider, UnicodeService );

		SERVICE_CREATE( m_serviceProvider, PluginSystem );
		SERVICE_CREATE( m_serviceProvider, PluginService );
		
		SERVICE_CREATE( m_serviceProvider, FileService );

		if( this->initializeFileEngine_() == false )
		{
			return false;
		}

		SERVICE_CREATE( m_serviceProvider, ConfigService );

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



		SERVICE_CREATE( m_serviceProvider, ArchiveService );	

		if( this->initializeArchiveService_() == false )
		{
			return false;
		}

		
		
		SERVICE_CREATE( m_serviceProvider, ThreadSystem );
		SERVICE_CREATE( m_serviceProvider, ThreadService );		

		SERVICE_CREATE( m_serviceProvider, NotificationService );

		SERVICE_CREATE( m_serviceProvider, RenderSystem );
		

		SERVICE_CREATE( m_serviceProvider, SoundSystem );
		
		bool muteMode = HAS_OPTION( m_serviceProvider, "mute" );
		
		if( muteMode == true || SERVICE_EXIST( m_serviceProvider, Menge::SoundSystemInterface ) == false )
		{
			SERVICE_CREATE( m_serviceProvider, SilentSoundSystem );
		}

		SERVICE_CREATE( m_serviceProvider, SoundService );

		SERVICE_CREATE( m_serviceProvider, ScriptService );
		SERVICE_CREATE( m_serviceProvider, ModuleService );
		SERVICE_CREATE( m_serviceProvider, CodecService );
		SERVICE_CREATE( m_serviceProvider, DataService );
		SERVICE_CREATE( m_serviceProvider, PrefetcherService );
		SERVICE_CREATE( m_serviceProvider, MemoryService );
		SERVICE_CREATE( m_serviceProvider, ConverterService );
		SERVICE_CREATE( m_serviceProvider, InputService );

		SERVICE_CREATE( m_serviceProvider, TimerSystem );
		SERVICE_CREATE( m_serviceProvider, TimerService );

#	ifdef _DEBUG
		{
			bool developmentMode = HAS_OPTION( m_serviceProvider, "dev" );
			bool roamingMode = HAS_OPTION( m_serviceProvider, "roaming" );
			bool noroamingMode = HAS_OPTION( m_serviceProvider, "noroaming" );

			if( developmentMode == true && roamingMode == false || noroamingMode == true )
			{
				WString userPath;
				this->makeUserPath_( userPath );

				CriticalErrorsMonitor::run( userPath, m_serviceProvider );
			}
		}
#	endif

		SERVICE_CREATE( m_serviceProvider, Application );

		// seed randomizer
		LARGE_INTEGER randomSeed;
		::QueryPerformanceCounter(&randomSeed);
		srand( randomSeed.LowPart );

		LOGGER_WARNING( m_serviceProvider )("initialize Application...");

#	define MENGINE_ADD_PLUGIN( Name, Info )\
		do{LOGGER_INFO(m_serviceProvider)( Info );\
		if(	PLUGIN_CREATE(m_serviceProvider, Name) == false ){\
		LOGGER_ERROR(m_serviceProvider)( "Invalid %s", Info );}else{\
		LOGGER_WARNING(m_serviceProvider)( "Successful %s", Info );}}while(false, false)

		MENGINE_ADD_PLUGIN( MengeImageCodec, "initialize Plugin Image Codec..." );
		MENGINE_ADD_PLUGIN( MengeSoundCodec, "initialize Plugin Sound Codec..." );
		MENGINE_ADD_PLUGIN( MengeOggVorbis, "initialize Plugin Ogg Vorbis Codec..." );
		MENGINE_ADD_PLUGIN( MengeAmplifier, "initialize Plugin Amplifier..." );
		MENGINE_ADD_PLUGIN( MengeVideoCodec, "initialize Plugin Video Codec..." );
		MENGINE_ADD_PLUGIN( MengeSpine, "initialize Plugin Spine..." );
		//MENGINE_ADD_PLUGIN(Motor, "initialize Plugin Motor...");
		MENGINE_ADD_PLUGIN( Box2D, "initialize Plugin Box2D..." );
		
		MENGINE_ADD_PLUGIN( PathFinder, "initialize Plugin Path Finder..." );


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

			if( PLUGIN_SERVICE(m_serviceProvider)
				->loadPlugin( pluginName ) == false )
			{
				LOGGER_ERROR(m_serviceProvider)("Application Failed to load plugin %ls"
					, pluginName.c_str()
					);

				return false;
			}
		}

#	ifdef _DEBUG
		bool devplugins = true;
#	else
		bool developmentMode = HAS_OPTION( m_serviceProvider, "dev" );
		bool devplugins = developmentMode;
#	endif
						
		bool nodevplugins = HAS_OPTION( m_serviceProvider, "nodevplugins" );

		if( devplugins == true && nodevplugins == false )
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

				if( PLUGIN_SERVICE(m_serviceProvider)
					->loadPlugin( pluginName ) == false )
				{
					LOGGER_WARNING(m_serviceProvider)("Application Failed to load dev plugin %ls"
						, pluginName.c_str()
						);
				}
			}
		}

		SERVICE_CREATE( m_serviceProvider, ParticleService );

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
			}
		}

		ConstString renderMaterialsPath = CONFIG_VALUE( m_serviceProvider, "Engine", "RenderMaterials", ConstString::none() );

		if( renderMaterialsPath.empty() == false )
		{
			if( RENDERMATERIAL_SERVICE( m_serviceProvider )
				->loadMaterials( ConstString::none(), renderMaterialsPath ) == false )
			{
				return false;
			}
		}

		LOGGER_INFO(m_serviceProvider)( "Application Create..."
			);

		ConstString resourceIniPath;
		if( this->getApplicationPath_( "Resource", "Path", resourceIniPath ) == false )
		{
			LOGGER_CRITICAL(m_serviceProvider)("Application invalid setup resource path"
				);

			return false;
		}

		if( APPLICATION_SERVICE( m_serviceProvider )->initializeGame( ConstString::none(), resourceIniPath ) == false )
		{
			LOGGER_CRITICAL(m_serviceProvider)("Application invalid initialize game"
				);

			return false;
		}

		LOGGER_INFO(m_serviceProvider)( "Creating Render Window..." );

		bool fullscreen = APPLICATION_SERVICE( m_serviceProvider )
			->getFullscreenMode();
		
		//if( m_application->isValidWindowMode() == false )
		//{
		//    fullscreen = true;
		//}

		if( WINDOWSLAYER_SERVICE(m_serviceProvider)->setProcessDPIAware() == false )
		{
			LOGGER_ERROR(m_serviceProvider)("Application not setup Process DPI Aware"
				);
		}

		const ConstString & projectTitle = APPLICATION_SERVICE( m_serviceProvider )
			->getProjectTitle();

		WString wprojectTitle;
		if( Helper::utf8ToUnicodeSize( m_serviceProvider, projectTitle.c_str(), projectTitle.size(), wprojectTitle ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("Application project title %s not convert to unicode"
				, projectTitle.c_str()
				);
		}

		Resolution windowResolution;
		APPLICATION_SERVICE( m_serviceProvider )
			->calcWindowResolution( windowResolution );

		if( PLATFORM_SERVICE( m_serviceProvider )
			->createWindow( IDI_MENGE, wprojectTitle, windowResolution, fullscreen ) == false )
		{
			return false;
		}
		
		//mt::vec2f point;
		//this->calcCursorPosition( point );

		//INPUT_SERVICE( m_serviceProvider )
		//	->setCursorPosition( 0, point );

		if( APPLICATION_SERVICE( m_serviceProvider )->createRenderWindow() == false )
		{
			LOGGER_CRITICAL(m_serviceProvider)("Application not create render window"
				);

			return false;
		}       

		bool vsync = APPLICATION_SERVICE( m_serviceProvider )
			->getVSync();

		bool maxfps = HAS_OPTION( m_serviceProvider, "maxfps" );
	
		if( maxfps == true && vsync == true )
		{
			APPLICATION_SERVICE( m_serviceProvider )
				->setVSync( false );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::loop()
	{
		PLATFORM_SERVICE( m_serviceProvider )
			->update();
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::finalize()
	{
		MODULE_SERVICE( m_serviceProvider )
			->stopModules();

		SERVICE_FINALIZE( m_serviceProvider, Menge::PlatformInterface );
		SERVICE_FINALIZE( m_serviceProvider, Menge::ApplicationInterface );
		SERVICE_FINALIZE( m_serviceProvider, Menge::PrefetcherServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, Menge::DataServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, Menge::PluginServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, Menge::ModuleServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, Menge::InputServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, Menge::UnicodeServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, Menge::UnicodeSystemInterface );

		SERVICE_FINALIZE( m_serviceProvider, Menge::FileServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, Menge::CodecServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, Menge::ParticleSystemInterface2 );
		SERVICE_FINALIZE( m_serviceProvider, Menge::ParticleServiceInterface2 );

		SERVICE_FINALIZE( m_serviceProvider, Menge::SoundServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, Menge::SoundSystemInterface );

		SERVICE_FINALIZE( m_serviceProvider, Menge::PrototypeServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, Menge::ScriptServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, Menge::ConverterServiceInterface );

		SERVICE_FINALIZE( m_serviceProvider, Menge::RenderSystemInterface );

		SERVICE_FINALIZE( m_serviceProvider, Menge::ConfigServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, Menge::StringizeServiceInterface );
		

		SERVICE_FINALIZE( m_serviceProvider, Menge::ArchiveServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, Menge::MemoryServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, Menge::NotificationServiceInterface );

		SERVICE_FINALIZE( m_serviceProvider, Menge::ThreadServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, Menge::ThreadSystemInterface );

		SERVICE_FINALIZE( m_serviceProvider, Menge::TimerServiceInterface );
		SERVICE_FINALIZE( m_serviceProvider, Menge::TimerSystemInterface );

		SERVICE_FINALIZE( m_serviceProvider, Menge::WindowsLayerInterface );

		if( m_fileLog != nullptr )
		{
			if( SERVICE_EXIST( m_serviceProvider, Menge::LoggerServiceInterface ) == true )
			{
				LOGGER_SERVICE( m_serviceProvider )
					->unregisterLogger( m_fileLog );
			}

			delete m_fileLog;
			m_fileLog = nullptr;
		}

		if( m_loggerConsole != nullptr )
		{
			if( SERVICE_EXIST( m_serviceProvider, Menge::LoggerServiceInterface ) == true )
			{
				LOGGER_SERVICE( m_serviceProvider )
					->unregisterLogger( m_loggerConsole );
			}

			delete m_loggerConsole;
			m_loggerConsole = nullptr;
		}

		if( m_loggerMessageBox != nullptr )
		{
			if( SERVICE_EXIST( m_serviceProvider, Menge::LoggerServiceInterface ) == true )
			{
				LOGGER_SERVICE( m_serviceProvider )
					->unregisterLogger( m_loggerMessageBox );
			}

			delete m_loggerMessageBox;
			m_loggerMessageBox = nullptr;
		}

		SERVICE_FINALIZE( m_serviceProvider, Menge::LoggerServiceInterface );

		SERVICE_PROVIDER_FINALIZE( m_serviceProvider );
		
		//::timeEndPeriod( 1 );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::update()
	{
		//
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
