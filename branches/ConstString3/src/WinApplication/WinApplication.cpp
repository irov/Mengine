#	include "Config/Config.h"

#	include "WinApplication.h"
#	include "Menge/Application.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/InputSystemInterface.h"

#	include <cstdio>
#	include <clocale>
#	include <shlobj.h>

#	include "LoggerConsole.h"
#	include "DynamicLibrary.h"
#	include "StringConversion.h"

#	include "FPSMonitor.h"
#	include "AlreadyRunningMonitor.h"
#	include "CriticalErrorsMonitor.h"

#	include "FileLogger.h"

#	include "resource.h"

#	include "SimpleIni.h"

#	include "Core/File.h"

#	include <ctime>
#	include <algorithm>

#	include <sstream>
#	include <iomanip>

//#	include <mhook.h>

#ifdef _MSC_VER
#	define snprintf _snprintf
#endif

namespace Menge
{
	static wchar_t s_logFileName[] = L"\\Game.log";
	static wchar_t s_userPath[MAX_PATH] = L"";

	//////////////////////////////////////////////////////////////////////////
	static const unsigned long s_activeFrameTime = 1000.f/60.f;
	static const unsigned long s_inactiveFrameTime = 100;
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
	//////////////////////////////////////////////////////////////////////////
	WinApplication::WinApplication( HINSTANCE _hInstance, const String& _commandLine ) 
		: m_running(true)
		, m_active(false)
		, m_name(L"Mengine")
		, m_windowClassName(L"MengeWnd")
		, m_hWnd(0)
		, m_cursorInArea(false)
		, m_hInstance(_hInstance)
		, m_loggerConsole(NULL)
		, m_commandLine(" " + _commandLine + " ")
		, m_application(NULL)
		, m_fpsMonitor(0)
		, m_alreadyRunningMonitor(0)
		, m_lastMouseX(0)
		, m_lastMouseY(0)
		, m_vsync(false)
		, m_maxfps(false)
		, m_cursorMode(false)
		, m_clipingCursor(FALSE)
		, m_windowsType(WindowsLayer::EWT_NT)
		, m_deadKey('\0')
		, m_logService(NULL)
		, m_winTimer(NULL)
		, m_isDoubleClick(false)
		, m_cursor(NULL)
		, m_enableDebug(false)
		, m_inputService(0)
		, m_developmentMode(false)
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
	void WinApplication::setDesktopResolution( const Resolution & _resolution )
	{
		m_desktopResolution = _resolution;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_getIniValue( const CSimpleIniCaseW & _ini, const wchar_t * _section, const wchar_t * _key, WString & _value )
	{
		const wchar_t * w_value = _ini.GetValue( _section, _key );

		if( w_value == NULL )
		{
			return false;
		}

		_value.assign( w_value );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_getIniValue( const CSimpleIniCaseW & _ini, const wchar_t * _section, const wchar_t * _key, String & _value )
	{
		const wchar_t * w_value = _ini.GetValue( _section, _key );

		if( w_value == NULL )
		{
			return false;
		}

		WindowsLayer::unicodeToAnsi( w_value, _value );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_getIniValue( const CSimpleIniCaseW & _ini, const wchar_t * _section, const wchar_t * _key, Resolution & _value )
	{
		const wchar_t * w_value = _ini.GetValue( _section, _key );

		if( w_value == NULL )
		{
			return false;
		}

		int width;
		int height;
		if( swscanf( w_value, L"%d;%d", &width, &height ) != 2 )
		{
			return false;
		}

		_value.setWidth( width );
		_value.setHeight( height );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_getIniValue( const CSimpleIniCaseW & _ini, const wchar_t * _section, const wchar_t * _key, Viewport & _value )
	{
		const wchar_t * w_value = _ini.GetValue( _section, _key );

		if( w_value == NULL )
		{
			return false;
		}

		float left;
		float top;
		float right;
		float bottom;
		if( swscanf( w_value, L"%f;%f;%f;%f", &left, &top, &right, &bottom ) != 4 )
		{
			return false;
		}

		_value.begin.x = left;
		_value.begin.y = top;
		_value.end.x = right;
		_value.end.y = bottom;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_getIniValue( const CSimpleIniCaseW & _ini, const wchar_t * _section, const wchar_t * _key, bool & _value )
	{
		const wchar_t * w_value = _ini.GetValue( _section, _key );

		if( w_value == NULL )
		{
			return false;
		}

		int tmp_value;
		if( swscanf( w_value, L"%d", &tmp_value ) != 1 )
		{
			return false;
		}

		_value = tmp_value != 0;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_getIniValue( const CSimpleIniCaseW & _ini, const wchar_t * _section, const wchar_t * _key, size_t & _value )
	{
		const wchar_t * w_value = _ini.GetValue( _section, _key );

		if( w_value == NULL )
		{
			return false;
		}

		size_t tmp_value;
		if( swscanf( w_value, L"%u", &tmp_value ) != 1 )
		{
			return false;
		}

		_value = tmp_value;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	static bool s_getIniValues( const CSimpleIniCaseW & _ini, const wchar_t * _section, const wchar_t * _key, TVectorWString & _values )
	{
		CSimpleIniCaseW::TNamesDepend values;
		if( _ini.GetAllValues( _section, _key, values ) == false )
		{
			return false;
		}

		for( CSimpleIniCaseW::TNamesDepend::const_iterator 
			it = values.begin(),
			it_end = values.end();
		it != it_end;
		++it )
		{
			_values.push_back( it->pItem );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool s_getIniAllSettings( const CSimpleIniCaseW & _ini, const wchar_t * _section, TMapWString & _values )
	{
		CSimpleIniCaseW::TNamesDepend values;
		if( _ini.GetAllKeys( _section, values ) == false )
		{
			return false;
		}

		for( CSimpleIniCaseW::TNamesDepend::const_iterator 
			it = values.begin(),
			it_end = values.end();
		it != it_end;
		++it )
		{
			WString val;
			s_getIniValue( _ini, _section, it->pItem, val );

			_values.insert( std::make_pair( it->pItem, val ) );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::initialize()
	{	
		CriticalErrorsMonitor::run( Application::getVersionInfo(), s_userPath, s_logFileName );

//
		//::timeBeginPeriod( 1 );

		//::MessageBoxA( NULL, "Starting debug", "Menge", MB_OK );
		WindowsLayer::setProcessDPIAware();

		m_windowsType = WindowsLayer::getWindowsType();

#	ifdef _DEBUG
		m_enableDebug = true;
#	else
		m_enableDebug = false;
#	endif

		if( m_enableDebug == false )
		{
			WindowsLayer::setModuleCurrentDirectory();
		}

		//String m_userPath("Antoinette");

		//HRSRC hResouce = ::FindResource( NULL, MAKEINTRESOURCE( 101 ), MAKEINTRESOURCE(10) );	//NULL, 101, RT_RCDATA
		//if( hResouce != NULL )
		//{
		//	DWORD resSize = ::SizeofResource( NULL, hResouce );
		//	HGLOBAL hResourceMem = ::LoadResource( NULL, hResouce );
		//	m_userPath.assign( reinterpret_cast<char*>( resSize, hResourceMem ) );
		//}

		//TODO - вынести это из компил€ции
		WindowsLayer::getCurrentDirectory( m_currentPath );
		m_currentPath += MENGE_FOLDER_DELIM;

		WString application_settings_path;
		application_settings_path += m_currentPath;
		application_settings_path += L"application.ini";

		printf( "load application config %S ..."
			, application_settings_path.c_str()
			);

		CSimpleIniCaseW ini_settings(false, true, true);
		SI_Error ini_error = ini_settings.LoadFile( application_settings_path.c_str() );

		if( ini_error != 0 )
		{
			::MessageBox( NULL
				, application_settings_path.c_str()
				, L"Invalid load application setting"
				, MB_OK
				);

			return false;
		}
		
		const wchar_t * w_game_path = ini_settings.GetValue( L"Game", L"Path" );

		if( w_game_path == 0 )
		{
			::MessageBox( NULL
				, application_settings_path.c_str()
				, L"Not found Game Path"
				, MB_OK
				);

			return false;
		}

		const wchar_t * w_resource_path = ini_settings.GetValue( L"Resource", L"Path" );

		if( w_resource_path == 0 )
		{
			::MessageBox( NULL
				, application_settings_path.c_str()
				, L"Not found Game Path"
				, MB_OK
				);

			return false;
		}
		
		CSimpleIniCaseW game_settings(false, true, true);

		WString game_settings_path;

		game_settings_path += m_currentPath;
		game_settings_path += w_game_path;

		SI_Error game_error = game_settings.LoadFile( game_settings_path.c_str() );

		if( game_error != 0 )
		{
			::MessageBox( NULL
				, game_settings_path.c_str()
				, L"Invalid load game setting"
				, MB_OK
				);

			return false;
		}

		ApplicationSettings appSettings;

        bool alreadyRunning = false;

        appSettings.fixedContentResolution = true;
        appSettings.windowModeCheck = false;
        appSettings.fullscreen = true;
        appSettings.vsync = true;

        String defaultLocale;
		
		s_getIniValue( game_settings, L"Project", L"Name", m_projectName );
        s_getIniValue( game_settings, L"Project", L"Company", m_companyName );
		s_getIniValue( game_settings, L"Project", L"Codename", appSettings.projectCodename );
        s_getIniValue( game_settings, L"Locale", L"Default", defaultLocale );
		s_getIniValue( game_settings, L"Game", L"ContentResolution", appSettings.contentResolution );
		s_getIniValue( game_settings, L"Game", L"LowContentViewport", appSettings.lowContentViewport );
		s_getIniValue( game_settings, L"Game", L"FixedContentResolution", appSettings.fixedContentResolution );
		s_getIniValue( game_settings, L"Game", L"PersonalityModule", appSettings.personalityModule );
        s_getIniValue( game_settings, L"Game", L"AlreadyRunning", alreadyRunning );
        s_getIniValue( game_settings, L"Game", L"WindowModeCheck", appSettings.windowModeCheck );
		s_getIniValue( game_settings, L"Window", L"Size", appSettings.windowResolution );
		s_getIniValue( game_settings, L"Window", L"Bits", appSettings.bits );
		s_getIniValue( game_settings, L"Window", L"Fullscreen", appSettings.fullscreen );
		s_getIniValue( game_settings, L"Window", L"VSync", appSettings.vsync );		

        TMapWString appParams;
		s_getIniAllSettings( game_settings, L"Params", appParams );

        TVectorWString plugins;
		s_getIniValues( game_settings, L"Plugins", L"Name", plugins );

		appSettings.platformName = "WIN";
		
		appSettings.baseDir = m_currentPath;

        if( m_projectName.empty() == true )
        {
            ::MessageBox( NULL
                , game_settings_path.c_str()
                , L"Invalid get project name from game setting"
                , MB_OK
                );

            return false;
        }

        if( m_companyName.empty() == true )
        {
            ::MessageBox( NULL
                , game_settings_path.c_str()
                , L"Invalid get company name from game setting"
                , MB_OK
                );

            return false;
        }

		//WString resourcePacksPath;
		//resourcePacksPath += m_currentPath;
		//resourcePacksPath += MENGE_FOLDER_DELIM;
		//resourcePacksPath += w_resource_path;

		CSimpleIniCaseW resources_settings(false, true, true);

		WString resources_settings_path;

		resources_settings_path += m_currentPath;
		resources_settings_path += w_resource_path;

		SI_Error resources_error = resources_settings.LoadFile( resources_settings_path.c_str() );
		
		if( resources_error != 0 )
		{
			::MessageBox( NULL
				, resources_settings_path.c_str()
				, L"Invalid load resources setting"
				, MB_OK
				);

			return false;
		}

		TVectorWString resourcePacksSettings;
		s_getIniValues( resources_settings, L"GAME_RESOURCES", L"ResourcePack", resourcePacksSettings );

		TVectorResourcePackDesc resourcePacks;

		for( TVectorWString::iterator
			it = resourcePacksSettings.begin(),
			it_end = resourcePacksSettings.end();
		it != it_end;
		++it )
		{
			const WString & resourcePack = *it;

			ResourcePackDesc pack;

			pack.preload = true;
			pack.type = "dir";
			
            s_getIniValue( resources_settings, resourcePack.c_str(), L"Name", pack.name );
			s_getIniValue( resources_settings, resourcePack.c_str(), L"Type", pack.type );            
			s_getIniValue( resources_settings, resourcePack.c_str(), L"Path", pack.path );
			s_getIniValue( resources_settings, resourcePack.c_str(), L"Locale", pack.locale );
			s_getIniValue( resources_settings, resourcePack.c_str(), L"Platform", pack.platform );
			s_getIniValue( resources_settings, resourcePack.c_str(), L"Description", pack.description );
			s_getIniValue( resources_settings, resourcePack.c_str(), L"PreLoad", pack.preload );						

			resourcePacks.push_back( pack );
		}

		TVectorWString languagePackSettings;
		s_getIniValues( resources_settings, L"GAME_RESOURCES", L"LanguagePack", languagePackSettings );

		TVectorResourcePackDesc languagePacks;

		for( TVectorWString::iterator
			it = languagePackSettings.begin(),
			it_end = languagePackSettings.end();
		it != it_end;
		++it )
		{
			const WString & resourcePack = *it;

			ResourcePackDesc pack;

			pack.preload = true;
			pack.type = "dir";			

			s_getIniValue( resources_settings, resourcePack.c_str(), L"Type", pack.type );
			s_getIniValue( resources_settings, resourcePack.c_str(), L"Name", pack.name );
			s_getIniValue( resources_settings, resourcePack.c_str(), L"Path", pack.path );
			s_getIniValue( resources_settings, resourcePack.c_str(), L"Locale", pack.locale );
			s_getIniValue( resources_settings, resourcePack.c_str(), L"Platform", pack.platform );
			s_getIniValue( resources_settings, resourcePack.c_str(), L"Description", pack.description );
			s_getIniValue( resources_settings, resourcePack.c_str(), L"PreLoad", pack.preload );	

			languagePacks.push_back( pack );
		}

		//ConstString languagePack;
		//if( cfg.getSetting( L"LOCALE", L"Default", languagePack ) == true )
		//{
		//	this->setLanguagePackOverride( languagePack );
		//}

		//cfg.getSetting( L"GamePack", L"Name", m_gamePackName );
		//cfg.getSetting( L"GamePack", L"Path", m_gamePackPath );
		//cfg.getSetting( L"GamePack", L"Description", m_gameDescription );
		//cfg.getSetting( L"GamePack", L"Type", m_gamePackType, Consts::get()->c_dir );

		//cfg.getSettings( L"Plugins", L"Name", m_initPlugins );

		//if( m_userPath.empty() == false )
		//{
		//	docsAndSettings = true;
		//}

		if( Helper::s_hasOption( " -dev ", m_commandLine ) == true )	// create user directory in ./User/
		{
			m_developmentMode = true;
		}

		String scriptInit;
		Helper::s_getOption( " -s:", m_commandLine, &scriptInit );
		
		String languagePack;
		Helper::s_getOption( " -lang:", m_commandLine, &languagePack );

        if( languagePack.empty() == true )
        {
            languagePack = defaultLocale;
        }

		if( languagePack.empty() == true )
		{
			wchar_t localeBuff[64];
			int localeBuffSize = ::GetLocaleInfo( LOCALE_USER_DEFAULT, LOCALE_SABBREVLANGNAME, localeBuff, 64 );
			WString languagePackW = std::wstring(localeBuff, localeBuffSize);

			WindowsLayer::unicodeToAnsi( languagePackW, languagePack );

			std::transform( languagePack.begin(), languagePack.end(), 
				languagePack.begin(), std::ptr_fun( &::tolower ) );
		}

		if( Helper::s_hasOption( " -maxfps ", m_commandLine ) == true )
		{
			m_maxfps = true;
		}


        if( alreadyRunning == true )
        {	
            m_alreadyRunningMonitor = new AlreadyRunningMonitor(m_logService);
            if( m_alreadyRunningMonitor->run( EARP_SETFOCUS, m_windowClassName, m_projectName ) == false )
            {
                return false;
            }
        }

		m_winTimer = new WinTimer();

		// seed randomizer
		LARGE_INTEGER randomSeed;
		::QueryPerformanceCounter(&randomSeed);
		srand( randomSeed.LowPart );

		setlocale( LC_CTYPE, "" );
		if( m_commandLine.find( " -console " ) != String::npos )
		{
			m_loggerConsole = new LoggerConsole();

			if( m_windowsType != WindowsLayer::EWT_98 )
			{
				m_loggerConsole->createConsole();
			}
		}

		//WindowsLayer::getCurrentDirectory( m_applicationPath );

		if( initInterfaceSystem( &m_application ) == false )
		{
			return false;
		}
	
		//m_application = new Application( this, m_logSystem, applicationPath, uUserPath, platformName );

		
		//LOG( "Enumarating monitors..." );
		//EnumDisplayMonitors( NULL, NULL, &s_monitorEnumProc, (LPARAM)this );

		int cxscreen = ::GetSystemMetrics( SM_CXSCREEN );
		int cyscreen = ::GetSystemMetrics( SM_CYSCREEN );
		m_desktopResolution.setWidth( cxscreen );
		m_desktopResolution.setHeight( cyscreen );

		m_application->setDesktopResolution( m_desktopResolution );

		RECT workArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

		RECT clientArea = workArea;
		::AdjustWindowRect( &clientArea, WS_OVERLAPPEDWINDOW, FALSE );
		size_t maxClientWidth = 2 * (workArea.right - workArea.left) - (clientArea.right - clientArea.left);
		size_t maxClientHeight = 2 * (workArea.bottom - workArea.top) - (clientArea.bottom - clientArea.top);

		Resolution maxClientResolution(maxClientWidth, maxClientHeight);
		m_application->setMaxClientResolution( maxClientResolution );

		String platformName = "WIN";

		//WString baseDir = m_applicationPath;
		//baseDir += MENGE_DEFAULT_BASE_DIR;
		
		//WString settings_file = L"application.ini";
        
		if( m_application->initialize( this, m_commandLine, appSettings ) == false )
		{
			LOGGER_ERROR(m_logService)( "Application initialize failed" 
				);

			return false;
		}

        if( m_application->loadPlugins( plugins ) == false )
        {
            MENGE_LOG_ERROR("Application Failed to loadPlugins");

            return false;
        }

        ConstString c_languagePack(languagePack);
        
        if( m_application->createGame( appSettings.personalityModule, c_languagePack, resourcePacks, languagePacks ) == false )
        {
            LOGGER_ERROR(m_logService)( "Application create game failed"
                );

            return false;
        }
				
		LOGGER_INFO(m_logService)( "Application Initialize... %s"
			, platformName.c_str() 
			);
		
		const WString & title = m_application->getProjectTitle();

		bool screenSaverMode = this->isSaverRunning();

		if( screenSaverMode == true )
		{
			String lowerCmdLine = m_commandLine.substr();
			std::transform( lowerCmdLine.begin(), lowerCmdLine.end(), lowerCmdLine.begin(), std::ptr_fun( ::tolower ) );

			if( lowerCmdLine.find(" /p") != String::npos || m_commandLine.find(" -p") != String::npos )
			{
				return false;
			}
			else if( lowerCmdLine.find(" /c") != String::npos || m_commandLine.find(" -c") != String::npos )
			{
				if( WindowsLayer::messageBox( m_hWnd, L"This screensaver has no options you can set\nDo you want to launch game?", title, MB_YESNO ) == IDNO )
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
		LOGGER_INFO(m_logService)( "Date: %02d.%02d.%d, %02d:%02d:%02d"
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
		LOGGER_INFO(m_logService)( "OS: Windows %ld.%ld.%ld %S"
			, os_ver.dwMajorVersion
			, os_ver.dwMinorVersion
			, os_ver.dwBuildNumber 
			, os_ver.szCSDVersion
			);

		MEMORYSTATUS mem_st;
		GlobalMemoryStatus(&mem_st);
		LOGGER_INFO(m_logService)( "Memory: %ldK total, %ldK free, %ldK Page file total, %ldK Page file free"
			, mem_st.dwTotalPhys / 1024L
			, mem_st.dwAvailPhys / 1024L
			, mem_st.dwTotalPageFile / 1024L
			, mem_st.dwAvailPageFile / 1024L 
			);

		const char * versionInfo = Application::getVersionInfo();
		LOGGER_INFO(m_logService)( "SVN Revision: %s"
			, versionInfo
			);

        LOGGER_INFO(m_logService)( "Initializing Game data..." );

        if( m_application->initializeGame( scriptInit, appParams ) == false )
        {
            return false;
        }

        LOGGER_INFO(m_logService)( "Creating Render Window..." );

        bool fullscreen = m_application->getFullscreenMode();

        Resolution windowResolution;
        m_application->calcWindowResolution( windowResolution );

        WindowHandle wh = this->createWindow( title, windowResolution, fullscreen );

        this->setActive( true );

        if( fullscreen == true )
        {
            this->notifyWindowModeChanged( m_desktopResolution, true );
        }

        LOGGER_INFO(m_logService)( "Initializing Game data..." );

		if( m_application->createRenderWindow( wh, wh ) == false )
		{
			return false;
		}


		//m_application->setCursorMode( m_cursorMode );

		m_vsync = m_application->getVSync();

		if( m_maxfps == false )
		{
			m_fpsMonitor = new FPSMonitor();
			m_fpsMonitor->initialize();

			m_fpsMonitor->setActive( true );
			m_fpsMonitor->setFrameTime( s_activeFrameTime );
		}

		m_cursors[L"IDC_ARROW"] = LoadCursor( NULL, IDC_ARROW );
		m_cursors[L"IDC_UPARROW"] = LoadCursor( NULL, IDC_UPARROW );
		m_cursors[L"IDC_HAND"] = LoadCursor( NULL, IDC_HAND );
		m_cursors[L"IDC_HELP"] = LoadCursor( NULL, IDC_HELP );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::setupLogService( LogServiceInterface * _service )
	{
		m_logService = _service;

		if( m_loggerConsole != NULL )
		{
			m_logService->registerLogger( m_loggerConsole );
		}

		EMessageLevel m_logLevel;

		if( m_developmentMode == true )
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

		m_logService->setVerboseLevel( m_logLevel );

		if( Helper::s_hasOption( " -verbose ", m_commandLine ) == true )
		{
			m_logService->setVerboseLevel( LM_MAX );

			LOGGER_INFO(m_logService)( "Verbose logging mode enabled" );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::setupFileService( FileServiceInterface * _service )
	{
		FileServiceInterface * fileService = _service;

		// mount root		
		if( fileService->mountFileSystem( ConstString::none, m_currentPath, ConstString("dir"), false ) == false )
		{
			LOGGER_ERROR(m_logService)( "WinApplication: failed to mount application directory %S"
				, m_currentPath.c_str()
				);
		}
        
		m_tempPath.clear();
        m_userPath.clear();

		if( m_developmentMode == true )
		{			
            m_tempPath += m_currentPath;
            m_tempPath += L"Temp";
            m_tempPath += MENGE_FOLDER_DELIM;

			m_userPath += m_currentPath;
			m_userPath += L"User";
			m_userPath += MENGE_FOLDER_DELIM;

			wcsncpy( s_userPath, m_userPath.c_str(), MAX_PATH );
			//std::replace( uUserPath.begin(), uUserPath.end(), '\\', '/' );
		}
		else	// create user directory in ~/Local Settings/Application Data/<uUserPath>/
		{
            wchar_t lpTempPathBuffer[MAX_PATH];
            DWORD dwRetVal = GetTempPath(MAX_PATH, lpTempPathBuffer);
                        
            m_tempPath.assign(lpTempPathBuffer, dwRetVal);

            m_tempPath += m_companyName;
            m_tempPath += MENGE_FOLDER_DELIM;
            m_tempPath += m_projectName;
            m_tempPath += MENGE_FOLDER_DELIM;

			wchar_t buffer[MAX_PATH];
			LPITEMIDLIST itemIDList;
			HRESULT hr = SHGetSpecialFolderLocation( NULL,
				CSIDL_APPDATA | CSIDL_FLAG_CREATE, &itemIDList );
			BOOL result = SHGetPathFromIDListW( itemIDList, buffer );
			CoTaskMemFree( itemIDList );
            
			m_userPath = buffer;
			m_userPath += MENGE_FOLDER_DELIM;
            m_userPath += m_companyName;
            m_userPath += MENGE_FOLDER_DELIM;
			m_userPath += m_projectName;
			m_userPath += MENGE_FOLDER_DELIM;

			wcsncpy( s_userPath, m_userPath.c_str(), MAX_PATH );
			//std::replace( uUserPath.begin(), uUserPath.end(), '\\', '/' );
		}

        // mount user directory
        if( fileService->mountFileSystem( ConstString("temp"), m_tempPath, ConstString("dir"), true ) == false )
        {
            LOGGER_ERROR(m_logService)( "WinApplication: failed to mount user directory %S"
                , m_tempPath.c_str()
                );
        }

		// mount user directory
		if( fileService->mountFileSystem( ConstString("user"), m_userPath, ConstString("dir"), true ) == false )
		{
			LOGGER_ERROR(m_logService)( "WinApplication: failed to mount user directory %S"
				, m_userPath.c_str()
				);
		}

		String logFilename = "Game";

		if( m_enableDebug == true )
		{
			std::stringstream dateStream;
			std::time_t ctTime; 
			std::time(&ctTime);
			std::tm* sTime = std::localtime( &ctTime );
			dateStream << 1900 + sTime->tm_year << "_" << std::setw(2) << std::setfill('0') <<
				(sTime->tm_mon+1) << "_" << std::setw(2) << std::setfill('0') << sTime->tm_mday << "_"
				<< std::setw(2) << std::setfill('0') << sTime->tm_hour << "_" 
				<< std::setw(2) << std::setfill('0') << sTime->tm_min << "_"
				<< std::setw(2) << std::setfill('0') << sTime->tm_sec;

			String dateString = dateStream.str();
			logFilename += "_";
			logFilename += dateString;
		}

		logFilename += ".log";

		WString logFilenameW;
		WindowsLayer::ansiToUnicode( logFilename, logFilenameW );

		FileOutputStreamInterface* fileLogInterface = fileService->openOutputFile( ConstString("user"), logFilenameW );

		m_fileLog = new FileLogger();
		m_fileLog->setFileInterface( fileLogInterface );

		if( fileLogInterface != NULL )
		{
			m_logService->registerLogger( m_fileLog );

			LOGGER_INFO(m_logService)("WinApplication: Starting log to %S"
				, logFilename.c_str()
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::setupInputService( InputServiceInterface * _service )
	{
		m_inputService = _service;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::loop()
	{
		while( m_running )
		{
			bool rendered = false;
			if( m_application->isFocus() == true )
			{
				rendered = m_application->onRender();
			}

			MSG  msg;
			while( WindowsLayer::peekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				::TranslateMessage( &msg );
				::WindowsLayer::dispatchMessage( &msg );
			}

			float frameTime = m_winTimer->getDeltaTime();

			bool updating = m_application->onUpdate();

			if( updating == true )
			{
				m_application->onTick( frameTime );
			}
			else
			{
				Sleep(100);
			}

			if( m_vsync == false )
			{
				if( m_fpsMonitor )
				{
					m_fpsMonitor->monitor();
				}
			}

			if( rendered )
			{
				m_application->onFlush();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::finalize()
	{
		// Clean up
		if( m_hWnd )
		{
			WindowsLayer::destroyWindow( m_hWnd );
			m_hWnd = NULL;
		}

		if( m_hInstance )
		{
			WindowsLayer::unregisterClass( m_windowClassName, m_hInstance );
		}	

		if( m_fpsMonitor != NULL )
		{
			m_fpsMonitor->finalize();
			delete m_fpsMonitor;
			m_fpsMonitor = 0;
		}
        
		if( m_application != NULL )
		{
			m_application->finalize();
		}

		if( m_fileLog != NULL )
		{
			if( m_logService )
			{
				m_logService->unregisterLogger( m_fileLog );
			}

			FileOutputStreamInterface * fileLogInterface = m_fileLog->getFileInterface();
			if( fileLogInterface != NULL )
			{
				fileLogInterface->close();
			}

			delete m_fileLog;
			m_fileLog = NULL;
		}

		if( m_loggerConsole != NULL )
		{
			if( m_logService )
			{
				m_logService->unregisterLogger( m_loggerConsole );
			}
			
			delete m_loggerConsole;
			m_loggerConsole = NULL;
		}

		if( m_application != NULL )
		{
			m_application->destroy();

			releaseInterfaceSystem( m_application );

			m_application = NULL;
		}



		if( m_alreadyRunningMonitor != NULL )
		{
			m_alreadyRunningMonitor->stop();
			delete m_alreadyRunningMonitor;
			m_alreadyRunningMonitor = 0;
		}

		if( m_winTimer != NULL )
		{
			delete static_cast<WinTimer*>(m_winTimer);
		}

		//::timeEndPeriod( 1 );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::stop()
	{
		m_running = false;
		if( m_hWnd != 0 )
		{
			CloseWindow( m_hWnd );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static LRESULT CALLBACK s_wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		switch( uMsg )
		{
		case WM_NCCREATE:
		case WM_CREATE:
			{
				WinApplication * app = (WinApplication *)WindowsLayer::getCreateParams( lParam );
				WindowsLayer::setWindowLongPtr( hWnd, GWL_USERDATA, (LONG_PTR)app );
			}
			break;
		}

		WinApplication * app = (WinApplication*)WindowsLayer::getWindowLongPtr( hWnd, GWL_USERDATA );

		if( app )
		{
			return app->wndProc( hWnd, uMsg, wParam, lParam );
		}

		return WindowsLayer::defWindowProc( hWnd, uMsg, wParam, lParam );
	}
	//////////////////////////////////////////////////////////////////////////
	WindowHandle WinApplication::createWindow( const Menge::WString & _name, const Resolution & _resolution, bool _fullscreen )
	{
		m_windowResolution = _resolution;

		m_name = _name;

		// Register the window class		
		ATOM result = WindowsLayer::registerClass( s_wndProc, 0, 0, m_hInstance, IDI_MENGE
					, (HBRUSH)GetStockObject(BLACK_BRUSH)
					, m_windowClassName
					);

		if( result == FALSE )
		{
			MENGE_LOG_ERROR("Can't register window class");
			return false;
		}

		DWORD dwStyle = this->getWindowStyle( _fullscreen );
		RECT rc = this->getWindowsRect( m_windowResolution, _fullscreen );

		DWORD exStyle = _fullscreen ? WS_EX_TOPMOST : 0;
		m_hWnd = WindowsLayer::createWindowEx( exStyle, m_windowClassName, m_name, dwStyle
				, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top
				, NULL, NULL, m_hInstance, (LPVOID)this );

		::ShowWindow( m_hWnd, SW_SHOW );

		return static_cast<WindowHandle>( m_hWnd ); 
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
	{
		m_windowResolution = _resolution;

		DWORD dwStyle = this->getWindowStyle( _fullscreen );
		RECT rc = this->getWindowsRect( m_windowResolution, _fullscreen );

		WindowsLayer::setWindowLong( m_hWnd, GWL_STYLE, dwStyle );

		DWORD dwExStyle = WindowsLayer::getWindowLong( m_hWnd, GWL_EXSTYLE );

		if( _fullscreen == false )
		{
			// When switching back to windowed mode, need to reset window size 
			// after device has been restored
			
			WindowsLayer::setWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyle & (~WS_EX_TOPMOST) );

			SetWindowPos(
				m_hWnd
				, HWND_NOTOPMOST
				, rc.left
				, rc.top
				, rc.right - rc.left
				, rc.bottom - rc.top
				, SWP_FRAMECHANGED // SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOACTIVATE
				);
		}
		else
		{
			WindowsLayer::setWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyle | WS_EX_TOPMOST );

			SetWindowPos( 
				m_hWnd
				, HWND_TOPMOST
				, rc.left
				, rc.top
				, rc.right - rc.left
				, rc.bottom - rc.top
				, SWP_FRAMECHANGED //SWP_NOZORDER | SWP_NOACTIVATE  | SWP_FRAMECHANGED
				);
		}

		::UpdateWindow( m_hWnd );
		::ShowWindow( m_hWnd, SW_SHOW );
	}
	//////////////////////////////////////////////////////////////////////////
	DWORD WinApplication::getWindowStyle( bool _fullsreen )
	{
		DWORD dwStyle = WS_POPUP;

		if( _fullsreen == false )
		{
			dwStyle |= WS_CLIPCHILDREN | WS_BORDER | WS_CAPTION | WS_VISIBLE;

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
	RECT WinApplication::getWindowsRect( const Resolution & _resolution, bool _fullsreen )
	{
		RECT rc;
		SetRect(&rc, 0, 0, (int)_resolution.getWidth(), (int)_resolution.getHeight());

		if( _fullsreen == false )
		{
			DWORD dwStyle = this->getWindowStyle( _fullsreen );
			AdjustWindowRect( &rc, dwStyle, FALSE );

			RECT workArea;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

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

		if( m_clipingCursor == TRUE )
		{
			ClipCursor( (m_active)?(&m_clipingCursorRect):NULL );
		}
		
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
			m_fpsMonitor->setFrameTime( (m_active)?s_activeFrameTime:s_inactiveFrameTime );
		}

		mt::vec2f point;
		this->getCursorPosition(point);

		m_inputService->onMousePosition( 0, point );

		m_application->onFocus( m_active, point );

		bool turnSound = m_active;
		m_application->onTurnSound( turnSound );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::getCursorPosition( mt::vec2f & _point )
	{
		POINT cPos;
		::GetCursorPos( &cPos );
		::ScreenToClient( m_hWnd, &cPos );

		_point.x = static_cast<float>(cPos.x);
		_point.y = static_cast<float>(cPos.y);
	}
	//////////////////////////////////////////////////////////////////////////
	LRESULT CALLBACK WinApplication::wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		if( m_hWnd != hWnd )
		{
			return WindowsLayer::defWindowProc( hWnd, uMsg, wParam, lParam );
		}

		//printf( "wndProc %x %x %x\n", uMsg, wParam, lParam );

		switch( uMsg )
		{
		//case WM_QUERYENDSESSION:
		//	{
		//		printf("WM_QUERYENDSESSION");
		//	}break;
		case WM_ACTIVATE:
			{
				bool active = (LOWORD(wParam) != WA_INACTIVE) && (HIWORD(wParam) == 0);

				//printf("WM_ACTIVATE m_active %d\n", active); 

				//POINT cPos;
				//::GetCursorPos( &cPos );
				//::ScreenToClient( m_hWnd, &cPos );

				//mt::vec2f point;
				//point.x = static_cast<float>(cPos.x);
				//point.y = static_cast<float>(cPos.y);

				//m_application->setCursorPosition( point );
				//m_application->pushMouseMoveEvent( 0, 0, 0 );

				this->setActive( active );

				return FALSE;
			}break;
		//case WM_NCACTIVATE:
		//	{
		//		if( m_active )
		//		{
		//			return FALSE;
		//		}
		//		//if( wParam == FALSE )
		//		//{
		//		//	return FALSE;
		//		//}
		//	}break;
		case WM_PAINT:
			{
				if( m_application->getFullscreenMode() == false )
				{
					m_application->onPaint();
				}
			}break;
		case WM_DISPLAYCHANGE:
			{
				//m_menge->onWindowMovedOrResized();
			}break;
		case WM_SIZE:
			if( m_hWnd != 0 )
			{
				if( wParam == SIZE_MAXIMIZED )
				{
					m_application->setFullscreenMode( true );

					setActive( true );
				}
				else if( wParam == SIZE_MINIMIZED )
				{
					setActive( false );
				}
				else if( wParam == SIZE_RESTORED && m_application->getFullscreenMode() == true )
				{
					setActive( true );
				}
			}break;
		case WM_GETMINMAXINFO:
			{
			// Prevent the window from going smaller than some minimu size
				((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
				((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
			}break;
		case WM_CLOSE:
			{
				m_application->onClose();
				return TRUE;
			}break;
		case WM_SYSKEYDOWN:
			{
				unsigned int vkc = static_cast<unsigned int>( wParam );
				HKL  layout = ::GetKeyboardLayout(0);
				unsigned int vk = MapVirtualKeyEx( vkc, 0, layout );

				mt::vec2f point;
				this->getCursorPosition(point);

				m_inputService->onKeyEvent( point, vkc, translateVirtualKey_( vkc, vk ), true );
			}break;
		case WM_SYSKEYUP:
			{
				unsigned int vkc = static_cast<unsigned int>( wParam );
				HKL  layout = ::GetKeyboardLayout(0);
				unsigned int vk = MapVirtualKeyEx( vkc, 0, layout );

				mt::vec2f point;
				this->getCursorPosition(point);

				m_inputService->onKeyEvent( point, vkc, 0, false );
			}break;
		case WM_SYSCOMMAND:
			if( (wParam & 0xFFF0) == SC_CLOSE )
			{
				m_active = false;				
			}
			else if( (wParam & 0xFFF0) == SC_KEYMENU )
			{
				if( lParam == 13 )
				{					
					bool fullscreen = m_application->getFullscreenMode();
					m_application->setFullscreenMode( !fullscreen );
				}

				return TRUE;
			}break;
		case WM_SETCURSOR:
			//printf( "set cursor\n" );
			{
				if( m_application->isFocus() && LOWORD(lParam) == HTCLIENT && m_cursorMode == false )
				{
					SetCursor(NULL);
				}
				else 
				{
					if (m_cursor == NULL)
					{
						m_cursor = LoadCursor(NULL, IDC_ARROW);						
					}

					::SetCursor( m_cursor );

					if( m_clipingCursor == TRUE )
					{
						ClipCursor( &m_clipingCursorRect );
					}
				}
				return FALSE;
			}break;
		case WM_DESTROY: 

			// Post the WM_QUIT message to 
			// quit the application terminate. 

			PostQuitMessage(0); 
			return 0;
		}

		this->wndProcInput( hWnd, uMsg, wParam, lParam );

		LRESULT result = WindowsLayer::defWindowProc( hWnd, uMsg, wParam, lParam );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::wndProcInput( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		switch( uMsg )
		{
		case WM_NCMOUSEMOVE:
		case WM_MOUSELEAVE:
			{
				if( m_cursorInArea == true )
				{
					m_cursorInArea = false;

					mt::vec2f point;
					this->getCursorPosition(point);
					m_inputService->onMousePosition( 0, point );

					m_application->onAppMouseLeave();
				}
			}break;
		case WM_MOUSEHOVER:
		case WM_MOUSEMOVE:
			{
				mt::vec2f point;
				this->getCursorPosition(point);

				if( m_cursorInArea == false )
				{
					m_cursorInArea = true;

					m_application->onAppMouseEnter( point );

					TRACKMOUSEEVENT mouseEvent = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, HOVER_DEFAULT };
					BOOL track = _TrackMouseEvent( &mouseEvent );
				}

				int x = (int)(short)LOWORD(lParam);
				int y = (int)(short)HIWORD(lParam);
				int dx = x - m_lastMouseX;
				int dy = y - m_lastMouseY;

				if( dx == 0 && dy == 0 )
				{
					break;
				}

				m_inputService->onMouseMove( 0, point, dx, dy, 0 );

				m_lastMouseX = x;
				m_lastMouseY = y;
			}break;
		case WM_MOUSEWHEEL:
			{
				int zDelta = static_cast<short>( HIWORD(wParam) );

				mt::vec2f point;
				this->getCursorPosition(point);				

				m_inputService->onMouseMove( 0, point, 0, 0, zDelta / WHEEL_DELTA );				
			}break;
		case WM_RBUTTONDBLCLK:
		case WM_LBUTTONDBLCLK:
			{
				m_isDoubleClick = true;
			}
			break;
		case WM_LBUTTONDOWN:
			{
				//printf("WM_LBUTTONDOWN m_active %d\n", m_active); 

				mt::vec2f point;
				this->getCursorPosition(point);

				m_inputService->onMouseButtonEvent( 0, point, 0, true );				
			}
			break;
		case WM_LBUTTONUP:
			{
				if( m_isDoubleClick == false )
				{
					mt::vec2f point;
					this->getCursorPosition(point);

					m_inputService->onMouseButtonEvent( 0, point, 0, false );					
				}

				m_isDoubleClick = false;
			}break;
		case WM_RBUTTONDOWN:
			{
				mt::vec2f point;
				this->getCursorPosition(point);

				m_inputService->onMouseButtonEvent( 0, point, 1, true );				
			}break;
		case WM_RBUTTONUP:
			{
				if( m_isDoubleClick == false )
				{
					mt::vec2f point;
					this->getCursorPosition(point);

					m_inputService->onMouseButtonEvent( 0, point, 1, false );
				}

				m_isDoubleClick = false;
			}break;
		case WM_MBUTTONDOWN:
			{
				mt::vec2f point;
				this->getCursorPosition(point);

				m_inputService->onMouseButtonEvent( 0, point, 2, true );				
			}break;
		case WM_MBUTTONUP:
			{
				mt::vec2f point;
				this->getCursorPosition(point);

				m_inputService->onMouseButtonEvent( 0, point, 2, false );				
			}break;
		case WM_KEYDOWN:
			{
				unsigned int vkc = static_cast<unsigned int>( wParam );
				HKL  layout = ::GetKeyboardLayout(0);
				unsigned int vk = MapVirtualKeyEx( vkc, 0, layout );

				mt::vec2f point;
				this->getCursorPosition(point);
				
				m_inputService->onKeyEvent( point, vkc, translateVirtualKey_( vkc, vk ), true );				
			}break;
		case WM_KEYUP:
			{
				unsigned int vkc = static_cast<unsigned int>( wParam );
				HKL  layout = ::GetKeyboardLayout(0);
				unsigned int vk = MapVirtualKeyEx( vkc, 0, layout );

				mt::vec2f point;
				this->getCursorPosition(point);

				m_inputService->onKeyEvent( point, vkc, 0, false );				
			}break;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution & WinApplication::getDesktopResolution() const
	{
		return m_desktopResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & WinApplication::getCurrentPath() const
	{
		return m_currentPath;
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
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::setCursorPosition( int _x, int _y )
	{
		m_lastMouseX = _x;
		m_lastMouseY = _y;
		POINT cPos = { _x, _y };
		ClientToScreen( m_hWnd, &cPos );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::showMessageBox( const WString& _message, const WString& _caption, unsigned int _style )
	{
		WindowsLayer::messageBox( m_hWnd, _message, _caption, MB_ICONERROR | MB_OK );
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
	DynamicLibraryInterface * WinApplication::loadDynamicLibrary( const WString& _filename )
	{
		DynamicLibrary * dynLib = new DynamicLibrary( _filename );

		if( dynLib->load() == false )
		{
			delete dynLib;

			return NULL;
		}

		return dynLib;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::unloadDynamicLibrary( DynamicLibraryInterface* _lib ) 
	{
		DynamicLibrary * dynLib = static_cast<DynamicLibrary*>( _lib );
		dynLib->unload();

		delete dynLib;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyCursorModeChanged( bool _mode )
	{
		m_cursorMode = _mode;

		if( m_cursor == NULL )
		{
			m_cursor = LoadCursor( NULL, IDC_ARROW );
		}

		m_cursor = m_cursorMode ? m_cursor : NULL;

		::SetCursor( m_cursor );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyCursorClipping( const Viewport & _viewport )
	{
		POINT p1 = { static_cast<LONG>( _viewport.begin.x ),
					 static_cast<LONG>( _viewport.begin.y ) };

		POINT p2 = { static_cast<LONG>( _viewport.end.x ),
					 static_cast<LONG>( _viewport.end.y ) };

		::ClientToScreen( m_hWnd, &p1 );
		::ClientToScreen( m_hWnd, &p2 );

		m_clipingCursorRect.left = p1.x;
		m_clipingCursorRect.top = p1.y;
		m_clipingCursorRect.right = p2.x;
		m_clipingCursorRect.bottom = p2.y;

		//printf( "%d %d - %d %d\n", p1.x, p1.y, p2.x, p2.y );
		
		m_clipingCursor = ClipCursor( &m_clipingCursorRect );	// Bound cursor
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyCursorUnClipping()
	{
		m_clipingCursor = FALSE;

		ClipCursor( NULL );
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		//////////////////////////////////////////////////////////////////////////
		class WinApplicationFileService
			: public ServiceListenerInterface
		{
		public:
			WinApplicationFileService( WinApplication * _application )
				: m_application(_application)
			{
			}

		protected:
			void onRegistryService( ServiceProviderInterface * _serviceProvide, ServiceInterface * _service ) override
			{
				m_application->setupFileService( static_cast<FileServiceInterface *>(_service) );		
			}

		protected:
			WinApplication * m_application;
		};
		//////////////////////////////////////////////////////////////////////////
		class WinApplicationLogService
			: public ServiceListenerInterface
		{
		public:
			WinApplicationLogService( WinApplication * _application )
				: m_application(_application)
			{
			}

		protected:
			void onRegistryService( ServiceProviderInterface * _serviceProvide, ServiceInterface * _service ) override
			{
				m_application->setupLogService( static_cast<LogServiceInterface*>(_service) );		
			}

		protected:
			WinApplication * m_application;
		};
		//////////////////////////////////////////////////////////////////////////
		class WinApplicationInputService
			: public ServiceListenerInterface
		{
		public:
			WinApplicationInputService( WinApplication * _application )
				: m_application(_application)
			{
			}

		protected:
			void onRegistryService( ServiceProviderInterface * _serviceProvide, ServiceInterface * _service ) override
			{
				m_application->setupInputService( static_cast<InputServiceInterface*>(_service) );		
			}

		protected:
			WinApplication * m_application;
		};
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyServiceProviderReady( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;

		m_serviceProvider->addServiceListener( "LogService", new WinApplicationLogService(this) );
		m_serviceProvider->addServiceListener( "FileService", new WinApplicationFileService(this) );
		m_serviceProvider->addServiceListener( "InputService", new WinApplicationInputService(this) );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyCursorIconSetup( const WString & _name, void * _buffer, size_t _size )
	{
		TMapCursors::iterator it_found = m_cursors.find( _name );

		if( it_found == m_cursors.end() )
		{
			WString icoDir;

			icoDir += m_userPath;
			icoDir += L"IconCache";

            WindowsLayer::createDirectory( icoDir );

            WString icoFile;

            icoFile += icoDir;
			icoFile += MENGE_FOLDER_DELIM;
			icoFile += _name;

			WString::size_type pos1 = icoFile.find_last_of( L'/' );
			WString::size_type pos2 = icoFile.find_last_of( L'\\' );

			WString::size_type pos = (std::max)(pos1, pos2);

			WString icoDir2 = icoFile.substr( 0, pos );

			WindowsLayer::createDirectory( icoDir2 );

			FILE * file = _wfopen( icoFile.c_str(), L"wb" );

			if( file == 0 )
			{
				LOGGER_ERROR(m_logService)("WinApplication::notifyCursorIconSetup can't create cursor cach file %S"
					, icoFile.c_str()
					);

				return;
			}

			fwrite( _buffer, _size, 1, file );
			fclose( file );
									
			//HCURSOR cursor = LoadCursorFromFile( _fileName.c_str() );
			HCURSOR cursor = LoadCursorFromFile( icoFile.c_str() );
			
			DWORD errCode = GetLastError();

			if( errCode != 0 )
			{
				LOGGER_ERROR(m_logService)("WinApplication::notifyCursorIconSetup errCode %d"
					, errCode 
					);

				return;
			}
			
			it_found = m_cursors.insert( std::make_pair( _name, cursor ) ).first;
		}
		
		m_cursor = it_found->second;

		SetCursor( m_cursor );
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
	bool WinApplication::isSaverRunning()
	{ 
		Menge::WString fileName;
		Menge::WString extention;
		WindowsLayer::getModuleFileName(NULL, fileName);
		if( fileName.length() < 4 )
		{
			return false;
		}

		extention = fileName.substr(fileName.length()-4);

		//std::transform( extention.begin(), extention.end(), extention.begin(), std::ptr_fun( ::tolower ) );
		/*for( int i=0; i<extention.length(); ++i )
		{
			extention[i] = tolower(extention[i]);
		}*/

		if( extention == L".scr" )
		{
			return true;
		}
		
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::setAsScreensaver( bool _set )
	{
		if( _set == true )
		{
			WString screensaverName = m_application->getScreensaverName();

			WString fullModuleName;
			WindowsLayer::getModuleFileName(NULL, fullModuleName);

			size_t separatorPos = fullModuleName.find_last_of(MENGE_FOLDER_DELIM);
			WString binFolderPath = fullModuleName.substr(0, separatorPos);
			WString fullScreensaverPath = binFolderPath + MENGE_FOLDER_DELIM + screensaverName;

			WString fullScreensaverPathShort;
			WindowsLayer::getShortPathName( fullScreensaverPath, fullScreensaverPathShort );

			WindowsLayer::setRegistryValue( HKEY_CURRENT_USER
				, L"Control Panel\\Desktop"
				, L"SCRNSAVE.EXE"
				, REG_SZ
				, reinterpret_cast<const BYTE*>( fullScreensaverPathShort.c_str() )
				, fullScreensaverPathShort.length() + 1 
				);
		}
		else
		{
			WindowsLayer::deleteRegistryValue( HKEY_CURRENT_USER, L"Control Panel\\Desktop", L"SCRNSAVE.EXE" );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::openUrlInDefaultBrowser( const WString & _url )
	{
		HINSTANCE result = ShellExecute( 0,L"open", _url.c_str() ,NULL,NULL,SW_NORMAL);
		int status = reinterpret_cast<int>(result);
		if(status > 32)
		{
			return true;
		}

		return false;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
