#	include "Config/Config.h"

#	include "WinApplication.h"
#	include "Menge/Application.h"
#	include "Interface/LogSystemInterface.h"

#	include <cstdio>
#	include <clocale>
#	include <shlobj.h>

#	include "LoggerConsole.h"
#	include "DynamicLibrary.h"
#	include "StringConversion.h"

#	include "FPSMonitor.h"
#	include "AlreadyRunningMonitor.h"
#	include "CriticalErrorsMonitor.h"

#	include "resource.h"

#	include "Menge/Utils.h"

#	include <ctime>
#	include <algorithm>

#ifdef _MSC_VER
#	define snprintf _snprintf
#endif

#	ifndef MENGE_MASTER_RELEASE
#		define LOG( message )\
	m_application->logMessage( message + String("\n"), LM_LOG );
#	else
#		define LOG( message )
#	endif

#	define LOG_ERROR( message )\
	m_application->logMessage( message + String("\n"), LM_ERROR );

typedef BOOL (WINAPI *PSETPROCESSDPIAWARE)(VOID);
 
namespace Menge
{
	static WCHAR s_logFileName[] = L"\\Game.log";
	static WCHAR s_userPath[MAX_PATH] = L"";

	//////////////////////////////////////////////////////////////////////////
	static const unsigned long s_activeFrameTime = 1000.f/60.f;
	static const unsigned long s_inactiveFrameTime = 100;
	//////////////////////////////////////////////////////////////////////////
	static BOOL CALLBACK s_monitorEnumProc( HMONITOR _hMonitor, HDC _hdc, LPRECT, LPARAM lParam )
	{
		WinApplication * app = reinterpret_cast<WinApplication*>(lParam);

		MONITORINFO info;
		ZeroMemory( &info, sizeof( MONITORINFO ) );
		info.cbSize = sizeof( MONITORINFO );

		::GetMonitorInfo( _hMonitor, &info );
		if( info.dwFlags == MONITORINFOF_PRIMARY )
		{
			Resolution desktopResolution;
			desktopResolution.setWidth( info.rcMonitor.right - info.rcMonitor.left );
			desktopResolution.setHeight( info.rcMonitor.bottom - info.rcMonitor.top );
			app->setDesktopResolution( desktopResolution );

			return FALSE;
		}
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_setProcessDPIAware()
	{
		HMODULE hUser32 = ::LoadLibraryA( "user32.dll" );
		if( hUser32 != NULL )
		{
			PSETPROCESSDPIAWARE pSetProcessDPIAware = GetProcAddress( hUser32, "SetProcessDPIAware" );
			if( pSetProcessDPIAware != NULL )
			{
				pSetProcessDPIAware();
			}
			FreeLibrary( hUser32 );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	static EWindowsType s_getWindowsType()
	{
		EWindowsType result = EWT_NT; 
		OSVERSIONINFOEXA osvi;
		ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXA));

		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
		if( GetVersionExA((LPOSVERSIONINFOA)&osvi) == 0)
		{
			osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFOA);
			if ( GetVersionExA((LPOSVERSIONINFOA)&osvi) == 0)
			{
				return result;
			}
		}

		if( osvi.dwMajorVersion >= 6 )
		{
			result = EWT_VISTA;
		}
		else if( osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS ) // let's check Win95, 98, *AND* ME.
		{
			result = EWT_98;
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	static void s_setCurrentDirectoryForBinary( EWindowsType _winType )
	{
		if( _winType == EWT_98 )
		{
			char exeFilePath[MAX_PATH];
			::GetModuleFileNameA( NULL, exeFilePath, MAX_PATH );
			std::string exeFileDir( exeFilePath );
			std::string::size_type slashPos = exeFileDir.find_last_of( '\\' );
			exeFileDir = exeFileDir.substr( 0, slashPos );
			if( slashPos != std::string::npos )
			{
				::SetCurrentDirectoryA( exeFileDir.c_str() );
			}
		}
		else
		{
			wchar_t exeFilePath[MAX_PATH];
			::GetModuleFileNameW( NULL, exeFilePath, MAX_PATH );
			std::wstring exeFileDir( exeFilePath );
			std::wstring::size_type slashPos = exeFileDir.find_last_of( '\\' );
			exeFileDir = exeFileDir.substr( 0, slashPos );
			if( slashPos != std::wstring::npos )
			{
				::SetCurrentDirectoryW( exeFileDir.c_str() );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	WinApplication::WinApplication( HINSTANCE _hInstance, const String& _commandLine ) 
		: m_running(true)
		, m_active(false)
		, m_frameTime( 0.f )
		, m_name( "Mengine" )
		, m_hWnd(0)
		, m_cursorInArea( false )
		, m_hInstance( _hInstance )
		, m_loggerConsole( NULL )
		, m_commandLine( " " + _commandLine + " ")
		, m_application( NULL )
		, m_fpsMonitor(0)
		, m_alreadyRunningMonitor(0)
		, m_lastMouseX(0)
		, m_lastMouseY(0)
		, m_vsync(false)
		, m_maxfps(false)
		, m_cursorMode(false)
		, m_clipingCursor(FALSE)
		, m_windowsType(EWT_NT)
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
	bool WinApplication::initialize()
	{
		bool enableDebug = false;
		bool docsAndSettings = false;

		timeBeginPeriod( 1 );

		//::MessageBoxA( NULL, "Starting debug", "Menge", MB_OK );
		s_setProcessDPIAware();

		m_windowsType = s_getWindowsType();

		CriticalErrorsMonitor::run( Application::getVersionInfo(), s_userPath, s_logFileName );

		String uUserPath;

		HRSRC hResouce = ::FindResourceA( NULL, MAKEINTRESOURCEA( 101 ), MAKEINTRESOURCEA(10) );	//NULL, 101, RT_RCDATA
		if( hResouce != NULL )
		{
			DWORD resSize = ::SizeofResource( NULL, hResouce );
			HGLOBAL hResourceMem = ::LoadResource( NULL, hResouce );
			uUserPath.assign( reinterpret_cast<char*>( resSize, hResourceMem ) );
		}

		if( uUserPath.empty() == false )
		{
			docsAndSettings = true;
		}

		if( m_commandLine.find( " -dev " ) != String::npos )	// create user directory in ./User/
		{
			docsAndSettings = false;
			enableDebug = true;
		}

		if( enableDebug == false )
		{
			s_setCurrentDirectoryForBinary( m_windowsType );
		}

		if( docsAndSettings == false || m_windowsType == EWT_98 )
		{
			//uUserPath = "./User";
			::GetCurrentDirectory( MAX_PATH, s_userPath );
			wcsncat( s_userPath, L"\\User", MAX_PATH );
			uUserPath = StringConversion::wcharToUTF8( s_userPath );
			std::replace( uUserPath.begin(), uUserPath.end(), '\\', '/' );
		}
		else	// create user directory in ~/Local Settings/Application Data/<uUserPath>/
		{
			StringW wUserPath = StringConversion::utf8ToWChar( uUserPath );

			LPITEMIDLIST itemIDList;
			HRESULT hr = SHGetSpecialFolderLocation( NULL,
				CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,
				&itemIDList );
			BOOL result = SHGetPathFromIDList( itemIDList, s_userPath );
			CoTaskMemFree( itemIDList );

			wcsncat( s_userPath, wUserPath.c_str(), MAX_PATH );
			uUserPath = StringConversion::wcharToUTF8( s_userPath );
			std::replace( uUserPath.begin(), uUserPath.end(), '\\', '/' );
		}


		std::size_t pos = 0;
		std::size_t fpos = String::npos;
		String scriptInit;
		fpos = m_commandLine.find( " -s:", 0 );
		if( fpos != String::npos )
		{
			String substring = "";
			if( m_commandLine[fpos+4] == '\"' )
			{
				std::size_t qpos = m_commandLine.find( '\"', fpos+5 );
				substring = m_commandLine.substr( fpos+5, qpos-(fpos+5) );
			}
			else
			{
				std::size_t spos = m_commandLine.find( ' ', fpos+4 );
				substring = m_commandLine.substr( fpos+4, spos-(fpos+4) );
			}
			scriptInit = substring;
		}
		String languagePack;
		fpos = m_commandLine.find( " -lang:", 0 );
		if( fpos != String::npos )
		{
			String::size_type endPos = m_commandLine.find( ' ', fpos+1 );
			languagePack = m_commandLine.substr( fpos+7, endPos-(fpos+7) );
		}

		if( m_commandLine.find( " -maxfps " ) != String::npos )
		{
			m_maxfps = true;
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

			if( m_windowsType != EWT_98 )
			{
				m_loggerConsole->createConsole();
			}
		}

		m_application = new Application( this, uUserPath, scriptInit, m_loggerConsole );
		m_application->enableDebug( enableDebug );

		if( m_commandLine.find( " -verbose " ) != String::npos )
		{
			m_application->setLoggingLevel( LM_MAX );

			LOG( "Verbose logging mode enabled" );
		}

		
		if( languagePack.empty() == true )
		{
			int buflen = ::GetLocaleInfoA( LOCALE_USER_DEFAULT, LOCALE_SABBREVLANGNAME, NULL, 0 );
			char* localeBuf = new char[buflen+1];
			::GetLocaleInfoA( LOCALE_USER_DEFAULT, LOCALE_SABBREVLANGNAME, localeBuf, buflen + 1 );
			languagePack = std::string( localeBuf );
			std::transform( languagePack.begin(), languagePack.end(), 
				languagePack.begin(), std::ptr_fun( &::tolower ) );
			delete localeBuf;
		}
		m_application->setLanguagePack( languagePack );


		//LOG( "Enumarating monitors..." );
		//EnumDisplayMonitors( NULL, NULL, &s_monitorEnumProc, (LPARAM)this );
		m_desktopResolution[0] = ::GetSystemMetrics( SM_CXSCREEN );
		m_desktopResolution[1] = ::GetSystemMetrics( SM_CYSCREEN );

		m_application->setDesktopResolution( m_desktopResolution );

		RECT workArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

		RECT clientArea = workArea;
		::AdjustWindowRect( &clientArea, WS_OVERLAPPEDWINDOW, FALSE );
		size_t maxClientWidth = 2 * (workArea.right - workArea.left) - (clientArea.right - clientArea.left);
		size_t maxClientHeight = 2 * (workArea.bottom - workArea.top) - (clientArea.bottom - clientArea.top);
		m_application->setMaxClientAreaSize( maxClientWidth, maxClientHeight );

		LOG( "Initializing Mengine..." );
		LOG( "UserPath " + uUserPath );

		Menge::String config_file = "application.xml";

		if( m_application->initialize( config_file, m_commandLine, true ) == false )
		{
			return false;
		}

		//char temp_log_string[256];
		//sprintf( temp_log_string, "desktop resolution %dx%d", m_desktopWidth, m_desktopHeight );
		//LOG( temp_log_string );
		//sprintf( temp_log_string, "max client area %dx%d", maxClientWidth, maxClientHeight );
		//LOG( temp_log_string );

		SYSTEMTIME tm;
		GetLocalTime(&tm);
		char strbuffer[1024];
		std::sprintf( strbuffer, "Date: %02d.%02d.%d, %02d:%02d:%02d", tm.wDay, tm.wMonth, tm.wYear, tm.wHour, tm.wMinute, tm.wSecond );
		LOG( strbuffer );

		OSVERSIONINFO os_ver;
		os_ver.dwOSVersionInfoSize = sizeof(os_ver);
		GetVersionEx(&os_ver);
		std::sprintf( strbuffer, "OS: Windows %ld.%ld.%ld", os_ver.dwMajorVersion, os_ver.dwMinorVersion, os_ver.dwBuildNumber );
		LOG( strbuffer );

		MEMORYSTATUS mem_st;
		GlobalMemoryStatus(&mem_st);
		std::sprintf( strbuffer, "Memory: %ldK total, %ldK free, %ldK Page file total, %ldK Page file free"
			, mem_st.dwTotalPhys/1024L
			, mem_st.dwAvailPhys/1024L
			, mem_st.dwTotalPageFile/1024L
			, mem_st.dwAvailPageFile/1024L );
		LOG( strbuffer );

		sprintf( strbuffer, "SVN Revision: %s", Application::getVersionInfo() );
		LOG( strbuffer );

		String title = m_application->getProjectTitle();

		int policy = m_application->getAlreadyRunningPolicy();

		if( m_windowsType != EWT_98 && policy != EARP_NONE )
		{	
			m_alreadyRunningMonitor = new AlreadyRunningMonitor;
			if( m_alreadyRunningMonitor->run( policy, title ) == false )
			{
				return false;
			}
		}

		m_fpsMonitor = new FPSMonitor();
		m_fpsMonitor->initialize();

		LOG( "Creating Render Window..." );
		bool fullscreen = m_application->getFullscreenMode();

		const Resolution & resolution = m_application->getResolution();
		WindowHandle wh = this->createWindow( title, resolution, fullscreen );

		if( fullscreen == true )
		{
			this->notifyWindowModeChanged( m_desktopResolution, true );
		}

		if( m_application->createRenderWindow( wh, wh ) == false )
		{
			return false;
		}

		LOG( "Initializing Game data..." );
		if( m_application->initGame() == false )
		{
			return false;
		}
		//m_application->setCursorMode( m_cursorMode );

		m_vsync = m_application->getVSync();

		m_fpsMonitor->setActive( m_vsync == false );
		m_fpsMonitor->setFrameTime( s_activeFrameTime );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::loop()
	{
		MSG  msg;

		while( m_running )
		{
			bool rendered = false;
			if( m_application->isFocus() == true )
			{
				rendered = m_application->onRender();
			}

			while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}

			m_frameTime = m_winTimer->getDeltaTime();

			bool updating = m_application->onUpdate();

			if( updating == true )
			{
				m_application->onTick( m_frameTime );
			}
			else
			{
				Sleep(100);
			}

			m_fpsMonitor->monitor();

			if( rendered )
			{
				m_application->onFlush();
			}
		}
	}
	void WinApplication::finialize()
	{
		// Clean up
		if( m_hWnd )
		{
			::DestroyWindow( m_hWnd );
			m_hWnd = NULL;
		}

		if( m_hInstance )
		{
			switch( m_windowsType )
			{
			case EWT_98:
				{
					::UnregisterClassA( "MengeWnd", m_hInstance );
				}break;
			default:
				{
					::UnregisterClassW( L"MengeWnd", m_hInstance );
				}break;				
			}
		}	

		if( m_fpsMonitor != NULL )
		{
			m_fpsMonitor->finialize();
			delete m_fpsMonitor;
			m_fpsMonitor = 0;
		}

		if( m_application != NULL )
		{
			delete m_application;
			m_application = NULL;
		}

		if( m_loggerConsole != NULL )
		{
			delete m_loggerConsole;
			m_loggerConsole = NULL;
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

		::timeEndPeriod( 1 );
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
			LPCREATESTRUCT createStruct = (LPCREATESTRUCT)lParam;
			WinApplication * app = (WinApplication *)createStruct->lpCreateParams;
			SetWindowLongPtr( hWnd, GWL_USERDATA, (LONG_PTR)app );
			break;
		}

		WinApplication * app = (WinApplication*)GetWindowLongPtr( hWnd, GWL_USERDATA );

		if( app )
		{
			return app->wndProc( hWnd, uMsg, wParam, lParam );
		}

		return ::DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
	//////////////////////////////////////////////////////////////////////////
	WindowHandle WinApplication::createWindow( const Menge::String & _name, const Resolution & _resolution, bool _fullscreen )
	{
		m_windowResolution = _resolution;

		m_name = _name;

		// Register the window class
		switch( m_windowsType )
		{
		case EWT_98:
			{
				WNDCLASSA wc;
				ZeroMemory( &wc, sizeof(WNDCLASS) );
				wc.lpfnWndProc = s_wndProc;
				wc.hInstance = m_hInstance;
				wc.hIcon = LoadIcon( m_hInstance, MAKEINTRESOURCE(IDI_MENGE) );
				wc.hCursor = LoadCursor( NULL, IDC_ARROW );
				wc.lpszClassName = "MengeWnd";
				wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

				if( ::RegisterClassA( &wc ) == FALSE )
				{
					MENGE_LOG_ERROR("Can't register window class");
					return false;
				}
			}break;
		default:
			{
				WNDCLASSW wc;
				ZeroMemory( &wc, sizeof(WNDCLASS) );
				wc.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
				wc.lpfnWndProc = s_wndProc;
				wc.hInstance = m_hInstance;
				wc.hIcon = LoadIcon( m_hInstance, MAKEINTRESOURCE(IDI_MENGE) );
				wc.hCursor = LoadCursor( NULL, IDC_ARROW );
				wc.lpszClassName = L"MengeWnd";
				wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

				if( ::RegisterClassW( &wc ) == FALSE )
				{
					MENGE_LOG_ERROR("Can't register window class");
					return false;
				}
			}break;
		}

		DWORD dwStyle = this->getWindowStyle( _fullscreen );
		RECT rc = this->getWindowsRect( m_windowResolution, _fullscreen );

		if( _fullscreen == false )
		{
			//StringW nameW = Menge::Utils::AToW( m_name );

			//m_lastMouseX = left;
			//m_lastMouseY = top;
			// Create our main window
			// Pass pointer to self

			switch( m_windowsType )
			{
			case EWT_98:
				{
					m_hWnd = CreateWindowExA(
						0
						, "MengeWnd"
						, m_name.c_str()
						, dwStyle
						, rc.left
						, rc.top
						, rc.right - rc.left
						, rc.bottom - rc.top
						, NULL
						, NULL
						, m_hInstance
						, (LPVOID)this
						);
				}break;
			default:
				{
					StringW nameW = StringConversion::utf8ToWChar( m_name );

					m_hWnd = CreateWindowExW(
						0
						, L"MengeWnd"
						, nameW.c_str()
						, dwStyle
						, rc.left
						, rc.top
						, rc.right - rc.left
						, rc.bottom - rc.top
						, NULL
						, NULL
						, m_hInstance
						, (LPVOID)this
						);
				}break;
			}
		}
		else
		{
			switch( m_windowsType )
			{
			case EWT_98:
				{
					m_hWnd = CreateWindowExA(
						WS_EX_TOPMOST
						, "MengeWnd"
						, m_name.c_str()
						, dwStyle
						, rc.left
						, rc.top
						, rc.right - rc.left
						, rc.bottom - rc.top
						, NULL
						, NULL
						, m_hInstance
						, (LPVOID)this
						);
				}break;
			default:
				{
					StringW nameW = StringConversion::utf8ToWChar( m_name );

					m_hWnd = CreateWindowExW(
						WS_EX_TOPMOST
						, L"MengeWnd"
						, nameW.c_str()
						, dwStyle
						, rc.left
						, rc.top
						, rc.right - rc.left
						, rc.bottom - rc.top
						, NULL
						, NULL
						, m_hInstance
						, (LPVOID)this
						);
				}break;
			}
		}

		::ShowWindow( m_hWnd, SW_SHOW );

		::GetWindowInfo( m_hWnd, &m_wndInfo);

		return static_cast<WindowHandle>( m_hWnd ); 
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
	{
		m_windowResolution = _resolution;

		DWORD dwStyle = this->getWindowStyle( _fullscreen );
		RECT rc = this->getWindowsRect( m_windowResolution, _fullscreen );

		SetWindowLong( m_hWnd, GWL_STYLE, dwStyle );

		DWORD dwExStyle = GetWindowLong( m_hWnd, GWL_EXSTYLE );

		if( _fullscreen == false )
		{
			// When switching back to windowed mode, need to reset window size 
			// after device has been restored
			
			SetWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyle & (~WS_EX_TOPMOST) );

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
			SetWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyle | WS_EX_TOPMOST );

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

		::ShowWindow( m_hWnd, SW_SHOW );

		::GetWindowInfo( m_hWnd, &m_wndInfo);
	}
	//////////////////////////////////////////////////////////////////////////
	DWORD WinApplication::getWindowStyle( bool _fullsreen )
	{
		DWORD dwStyle = WS_POPUP;

		if( _fullsreen == false )
		{
			dwStyle |= WS_CLIPCHILDREN | WS_BORDER | WS_CAPTION | WS_VISIBLE;

			bool hasWindowPanel = m_application->getHasWindowPanel();

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

		m_fpsMonitor->setFrameTime( (m_active)?s_activeFrameTime:s_inactiveFrameTime );
		m_application->onFocus( m_active );

		if( m_clipingCursor == TRUE )
		{
			ClipCursor( (m_active)?(&m_clipingCursorRect):NULL );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	LRESULT CALLBACK WinApplication::wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		if( m_hWnd != hWnd )
		{
			return ::DefWindowProc( hWnd, uMsg, wParam, lParam );
		}

		//printf( "wndProc %x %x %x\n", uMsg, wParam, lParam );

		switch( uMsg )
		{
		case WM_ACTIVATE:
			{
				bool active = (LOWORD(wParam) != WA_INACTIVE) && (HIWORD(wParam) == 0);

				this->setActive( active );

				return FALSE;
			} break;
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
			}
			break;
		case WM_MOVE:
			if( m_hWnd != 0 )
			{
				::GetWindowInfo( m_hWnd, &m_wndInfo);
				//m_menge->onWindowMovedOrResized();
			}
			break;
		case WM_DISPLAYCHANGE:
			//m_menge->onWindowMovedOrResized();
			break;
		case WM_SIZE:
			if( m_hWnd != 0 )
			{
				::GetWindowInfo( m_hWnd, &m_wndInfo);
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
			}
			break;
		case WM_GETMINMAXINFO:
			// Prevent the window from going smaller than some minimu size
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
			break;
		case WM_CLOSE:
			m_application->onClose();
			return TRUE;
			break;
		case WM_SYSCOMMAND:
			if( wParam == SC_CLOSE )
			{
				m_active = false;				
			}
			else if( wParam == SC_KEYMENU )
			{
				if( lParam == 13 )
				{					
					bool fullscreen = m_application->getFullscreenMode();
					m_application->setFullscreenMode( !fullscreen );
				}

				return TRUE;
			}
			break;
		case WM_SETCURSOR:
			//printf( "set cursor\n" );
			{
				if( m_application->isFocus() && LOWORD(lParam) == HTCLIENT && m_cursorMode == false )
				{
					SetCursor(NULL);
				}
				else 
				{
					HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
					::SetCursor( cursor );

					if( m_clipingCursor == TRUE )
					{
						ClipCursor( &m_clipingCursorRect );
					}
				}
				return FALSE;
			}
			break;
		case WM_NCMOUSEMOVE:
		case WM_MOUSELEAVE:
			{
				if( m_cursorInArea == true )
				{
					m_cursorInArea = false;
					m_application->onMouseLeave();
				}
			}
			break;
		case WM_MOUSEMOVE:
			{
				if( m_cursorInArea == false )
				{
					m_cursorInArea = true;
					m_application->onMouseEnter();

					TRACKMOUSEEVENT mouseEvent = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, HOVER_DEFAULT };
					BOOL track = TrackMouseEvent( &mouseEvent );
				}

				int x = (int)(short)LOWORD(lParam);
				int y = (int)(short)HIWORD(lParam);
				int dx = x - m_lastMouseX;
				int dy = y - m_lastMouseY;
				m_application->injectMouseMove( dx, dy, 0 );
				POINT cPos;
				::GetCursorPos( &cPos );
				::ScreenToClient( m_hWnd, &cPos );
				m_application->setMousePosition( cPos.x, cPos.y );
				//printf("%d %d %d %d %d %d\n", cPos.x, cPos.y, m_lastMouseX, m_lastMouseY, cPos.x - m_lastMouseX, cPos.y - m_lastMouseY );
				m_lastMouseX = x;
				m_lastMouseY = y;

			}
			break;
		case WM_MOUSEWHEEL:
			{
				int zDelta = static_cast<short>( HIWORD(wParam) );
				m_application->injectMouseMove( 0, 0, zDelta );
			}
			break;
		case WM_LBUTTONDOWN:
			m_application->onMouseButtonEvent( 0, true );
			break;
		case WM_LBUTTONUP:
			m_application->onMouseButtonEvent( 0, false );
			break;
		case WM_RBUTTONDOWN:
			m_application->onMouseButtonEvent( 1, true );
			break;
		case WM_RBUTTONUP:
			m_application->onMouseButtonEvent( 1, false );
			break;
		case WM_MBUTTONDOWN:
			m_application->onMouseButtonEvent( 2, true );
			break;
		case WM_MBUTTONUP:
			m_application->onMouseButtonEvent( 2, false );
			break;
		}

		return ::DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
	//////////////////////////////////////////////////////////////////////////
	const Resolution & WinApplication::getDesktopResolution() const
	{
		return m_desktopResolution;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::minimizeWindow()
	{
		::ShowWindow( m_hWnd, SW_MINIMIZE );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyVsyncChanged( bool _vsync )
	{
		if( m_maxfps == true )
		{
			return;
		}

		m_vsync = _vsync;

		m_fpsMonitor->setActive( m_vsync == false );
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
	void WinApplication::showMessageBox( const String& _message, const String& _header, unsigned int _style )
	{
		StringW message_w = StringConversion::utf8ToWChar( _message );
		StringW header_w = StringConversion::utf8ToWChar( _header );

		::MessageBox( m_hWnd, message_w.c_str(), header_w.c_str(), MB_ICONERROR | MB_OK );
	}
	//////////////////////////////////////////////////////////////////////////
	Menge::String WinApplication::ansiToUtf8( const String& _ansi )
	{
		int wide_size = MultiByteToWideChar( CP_ACP, 0, _ansi.c_str(), -1, NULL, 0 );
		wchar_t* wide = new wchar_t[wide_size];
		MultiByteToWideChar( CP_ACP, 0, _ansi.c_str(), -1, wide, wide_size );
		int utf8_size = WideCharToMultiByte( CP_UTF8, 0, wide, wide_size, NULL, 0, NULL, NULL );
		char* utf8 = new char[utf8_size];
		WideCharToMultiByte( CP_UTF8, 0, wide, wide_size, utf8, utf8_size, NULL, NULL );
		String out( utf8 );
		delete[] wide;
		delete[] utf8;
		return out;
	}
	//////////////////////////////////////////////////////////////////////////
	Menge::String WinApplication::utf8ToAnsi( const String& _utf8 )
	{
		int wide_size = MultiByteToWideChar( CP_UTF8, 0, _utf8.c_str(), -1, NULL, 0 );
		wchar_t* wide = new wchar_t[wide_size];
		MultiByteToWideChar( CP_UTF8, 0, _utf8.c_str(), -1, wide, wide_size );
		int anis_size = WideCharToMultiByte( CP_ACP, 0, wide, wide_size, NULL, 0, NULL, NULL );
		char* ansi = new char[anis_size];
		WideCharToMultiByte( CP_ACP, 0, wide, wide_size, ansi, anis_size, NULL, NULL );
		String out( ansi );
		delete[] wide;
		delete[] ansi;
		return out;
	}
	//////////////////////////////////////////////////////////////////////////
	DynamicLibraryInterface * WinApplication::load( const String& _filename )
	{
		DynamicLibrary * dynLib = new DynamicLibrary( _filename );
		dynLib->load();

		return dynLib;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::unload( DynamicLibraryInterface* _lib ) 
	{
		DynamicLibrary * dynLib = static_cast<DynamicLibrary*>( _lib );
		dynLib->unload();

		delete dynLib;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyCursorModeChanged( bool _mode )
	{
		m_cursorMode = _mode;
		HCURSOR hCursor = m_cursorMode ? LoadCursor( NULL, IDC_ARROW ) : NULL;
		::SetCursor( hCursor );
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
		
		m_clipingCursor = ClipCursor( &m_clipingCursorRect );	// Bound cursor
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyCursorUnClipping()
	{
		m_clipingCursor = FALSE;

		ClipCursor( NULL );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

