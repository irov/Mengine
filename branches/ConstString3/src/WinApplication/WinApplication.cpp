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

#	include "Logger/Logger.h"
#	include "Core/File.h"

#	include <ctime>
#	include <algorithm>

#ifdef _MSC_VER
#	define snprintf _snprintf
#endif

#	ifndef MENGE_MASTER_RELEASE
#		define LOG( message )\
	m_application->logMessage( message + String("\n"), LM_INFO );
#	else
#		define LOG( message )
#	endif

#	define LOG_ERROR( message )\
	m_application->logMessage( message + String("\n"), LM_ERROR );
 
namespace Menge
{
	static char s_logFileName[] = "\\Game.log";
	static char s_userPath[MAX_PATH] = "";

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
		, m_frameTime(0.f)
		, m_name("Mengine")
		, m_hWnd(0)
		, m_cursorInArea(false)
		, m_hInstance(_hInstance)
		, m_logger(NULL)
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
		, m_logSystemInterface(NULL)
		, m_winTimer(NULL)
		, m_isDoubleClick(false)
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
		CriticalErrorsMonitor::run( Application::getVersionInfo(), s_userPath, s_logFileName );

		bool enableDebug = false;
		bool docsAndSettings = false;

		//::timeBeginPeriod( 1 );

		//::MessageBoxA( NULL, "Starting debug", "Menge", MB_OK );
		WindowsLayer::setProcessDPIAware();

		m_windowsType = WindowsLayer::getWindowsType();

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

		if( Helper::s_hasOption( " -dev ", m_commandLine ) == true )	// create user directory in ./User/
		{
			docsAndSettings = false;
			enableDebug = true;
		}

		if( enableDebug == false )
		{
			WindowsLayer::setModuleCurrentDirectory();
		}

		if( docsAndSettings == false || m_windowsType == WindowsLayer::EWT_98 )
		{
			WindowsLayer::getCurrentDirectory( &uUserPath );
			uUserPath += "\\User";
			strncpy( s_userPath, uUserPath.c_str(), MAX_PATH );
			std::replace( uUserPath.begin(), uUserPath.end(), '\\', '/' );
		}
		else	// create user directory in ~/Local Settings/Application Data/<uUserPath>/
		{
			wchar_t buffer[MAX_PATH];
			LPITEMIDLIST itemIDList;
			HRESULT hr = SHGetSpecialFolderLocation( NULL,
				CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, &itemIDList );
			BOOL result = SHGetPathFromIDListW( itemIDList, buffer );
			CoTaskMemFree( itemIDList );
			
			Menge::String userSysPath;
			WindowsLayer::wstrToUtf8( Menge::StringW( buffer ), &userSysPath );
			uUserPath = userSysPath + uUserPath;

			strncpy( s_userPath, uUserPath.c_str(), MAX_PATH );
			std::replace( uUserPath.begin(), uUserPath.end(), '\\', '/' );
		}


		String scriptInit;
		Helper::s_getOption( " -s:",m_commandLine, &scriptInit );
		
		String languagePack;
		Helper::s_getOption( " -lang:", m_commandLine, &languagePack );

		if( languagePack.empty() == true )
		{
			char localeBuff[64];
			int localeBuffSize = ::GetLocaleInfoA( LOCALE_USER_DEFAULT, LOCALE_SABBREVLANGNAME, localeBuff, 64 );
			languagePack = std::string(localeBuff, localeBuffSize);
			std::transform( languagePack.begin(), languagePack.end(), 
				languagePack.begin(), std::ptr_fun( &::tolower ) );
		}

		if( Helper::s_hasOption( " -maxfps ", m_commandLine ) == true )
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

			if( m_windowsType != WindowsLayer::EWT_98 )
			{
				m_loggerConsole->createConsole();
			}
		}

		bool result = initInterfaceSystem( &m_logSystemInterface );

		m_logger = new Logger;
		Logger::keep(m_logger);

		m_logger->initialize( m_logSystemInterface );

		if( m_loggerConsole != NULL )
		{
			m_logger->registerLogger( m_loggerConsole );
		}
	
		m_application = new Application( this, m_logger, uUserPath );

		Application::keep( m_application );

		m_application->enableDebug( enableDebug );
		
		if( enableDebug == true )
		{
			m_application->setLoggingLevel( LM_LOG );
		}

		if( m_commandLine.find( " -verbose " ) != String::npos )
		{
			m_application->setLoggingLevel( LM_MAX );

			LOG( "Verbose logging mode enabled" );
		}

		//LOG( "Enumarating monitors..." );
		//EnumDisplayMonitors( NULL, NULL, &s_monitorEnumProc, (LPARAM)this );

		int cxscreen = ::GetSystemMetrics( SM_CXSCREEN );
		int cyscreen = ::GetSystemMetrics( SM_CYSCREEN );
		m_desktopResolution.setWidth( cxscreen );
		m_desktopResolution.setHeight( cyscreen );

		m_application->setDesktopResolution( m_desktopResolution );

		RECT workArea;
		SystemParametersInfoA(SPI_GETWORKAREA, 0, &workArea, 0);

		RECT clientArea = workArea;
		::AdjustWindowRect( &clientArea, WS_OVERLAPPEDWINDOW, FALSE );
		size_t maxClientWidth = 2 * (workArea.right - workArea.left) - (clientArea.right - clientArea.left);
		size_t maxClientHeight = 2 * (workArea.bottom - workArea.top) - (clientArea.bottom - clientArea.top);
		m_application->setMaxClientAreaSize( maxClientWidth, maxClientHeight );

		LOG( "Initializing Mengine..." );
		LOG( "UserPath " + uUserPath );

		Menge::String config_file = "application";

		if( m_application->initialize( config_file, m_commandLine ) == false )
		{
			return false;
		}

		ConstString c_languagePack(languagePack);
		m_application->setLanguagePack( c_languagePack );

		if( m_application->loadGame( true ) == false )
		{
			return false;
		}

		const String & title = m_application->getProjectTitle();

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
				if( WindowsLayer::messageBox( m_hWnd, "This screensaver has no options you can set\nDo you want to launch game?", title, MB_YESNO ) == IDNO )
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
		char strbuffer[1024];
		std::sprintf( strbuffer, "Date: %02d.%02d.%d, %02d:%02d:%02d", tm.wDay, tm.wMonth, tm.wYear, tm.wHour, tm.wMinute, tm.wSecond );
		LOG( strbuffer );

		OSVERSIONINFOA os_ver;
		os_ver.dwOSVersionInfoSize = sizeof(os_ver);
		GetVersionExA(&os_ver);
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

		int policy = m_application->getAlreadyRunningPolicy();

		if( m_windowsType != WindowsLayer::EWT_98 && policy != EARP_NONE )
		{	
			m_alreadyRunningMonitor = new AlreadyRunningMonitor;
			if( m_alreadyRunningMonitor->run( policy, title ) == false )
			{
				return false;
			}
		}

		LOG( "Creating Render Window..." );
		bool fullscreen = m_application->getFullscreenMode();

		const Resolution & resolution = m_application->getResolution();
		WindowHandle wh = this->createWindow( title, resolution, fullscreen );

		if( fullscreen == true )
		{
			this->notifyWindowModeChanged( m_desktopResolution, true );
		}

		this->setActive( true );

		if( m_application->createRenderWindow( wh, wh ) == false )
		{
			return false;
		}

		LOG( "Initializing Game data..." );
		if( m_application->initGame( scriptInit ) == false )
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

			while( WindowsLayer::peekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				::TranslateMessage( &msg );
				::WindowsLayer::dispatchMessage( &msg );
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

			if( m_fpsMonitor )
			{
				m_fpsMonitor->monitor();
			}

			if( rendered )
			{
				m_application->onFlush();
			}
		}
	}
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
			WindowsLayer::unregisterClass( Menge::String( "MengeWnd" ), m_hInstance );
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
			delete m_application;
			m_application = NULL;
		}

		if( m_logger )
		{
			m_logger->unregisterLogger( m_loggerConsole );

			delete m_logger;
		}

		if( m_logSystemInterface != NULL )
		{
			releaseInterfaceSystem( m_logSystemInterface );
			m_logSystemInterface = NULL;
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
	WindowHandle WinApplication::createWindow( const Menge::String & _name, const Resolution & _resolution, bool _fullscreen )
	{
		m_windowResolution = _resolution;

		m_name = _name;

		// Register the window class
		ATOM result = WindowsLayer::registerClass( s_wndProc, 0, 0, m_hInstance, IDI_MENGE
					, (HBRUSH)GetStockObject(BLACK_BRUSH), NULL, Menge::String( "MengeWnd" ) );
		if( result == FALSE )
		{
			MENGE_LOG_ERROR("Can't register window class");
			return false;
		}

		DWORD dwStyle = this->getWindowStyle( _fullscreen );
		RECT rc = this->getWindowsRect( m_windowResolution, _fullscreen );

		DWORD exStyle = _fullscreen ? WS_EX_TOPMOST : 0;
		m_hWnd = WindowsLayer::createWindowEx( exStyle, "MengeWnd", m_name, dwStyle
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

		::ShowWindow( m_hWnd, SW_SHOW );
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
			SystemParametersInfoA(SPI_GETWORKAREA, 0, &workArea, 0);

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

		if( m_fpsMonitor )
		{
			m_fpsMonitor->setFrameTime( (m_active)?s_activeFrameTime:s_inactiveFrameTime );
		}

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
			return WindowsLayer::defWindowProc( hWnd, uMsg, wParam, lParam );
		}

		//printf( "wndProc %x %x %x\n", uMsg, wParam, lParam );

		switch( uMsg )
		{
		case WM_ACTIVATE:
			{
				bool active = (LOWORD(wParam) != WA_INACTIVE) && (HIWORD(wParam) == 0);

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
				unsigned int vk = MapVirtualKeyExA( vkc, 0, layout );
				m_application->pushKeyEvent( vkc, translateVirtualKey_( vkc, vk ), true );
			}break;
		case WM_SYSKEYUP:
			{
				unsigned int vkc = static_cast<unsigned int>( wParam );
				HKL  layout = ::GetKeyboardLayout(0);
				unsigned int vk = MapVirtualKeyExA( vkc, 0, layout );
				m_application->pushKeyEvent( vkc, 0, false );
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
					HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
					::SetCursor( cursor );

					if( m_clipingCursor == TRUE )
					{
						ClipCursor( &m_clipingCursorRect );
					}
				}
				return FALSE;
			}break;
		case WM_NCMOUSEMOVE:
		case WM_MOUSELEAVE:
			{
				if( m_cursorInArea == true )
				{
					m_cursorInArea = false;
					m_application->onAppMouseLeave();
				}
			}break;
		case WM_MOUSEHOVER:
		case WM_MOUSEMOVE:
			{
				if( m_cursorInArea == false )
				{
					m_cursorInArea = true;
					m_application->onAppMouseEnter();

					TRACKMOUSEEVENT mouseEvent = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, HOVER_DEFAULT };
					BOOL track = _TrackMouseEvent( &mouseEvent );
					int a;
					a = 0;
				}

				int x = (int)(short)LOWORD(lParam);
				int y = (int)(short)HIWORD(lParam);
				int dx = x - m_lastMouseX;
				int dy = y - m_lastMouseY;

				if( dx == 0 && dy == 0 )
				{
					break;
				}

				m_application->pushMouseMoveEvent( dx, dy, 0 );
				POINT cPos;
				::GetCursorPos( &cPos );
				::ScreenToClient( m_hWnd, &cPos );
				m_application->setMousePosition( cPos.x, cPos.y );
				//printf("%d %d %d %d %d %d\n", cPos.x, cPos.y, m_lastMouseX, m_lastMouseY, cPos.x - m_lastMouseX, cPos.y - m_lastMouseY );
				m_lastMouseX = x;
				m_lastMouseY = y;
			}break;
		case WM_MOUSEWHEEL:
			{
				int zDelta = static_cast<short>( HIWORD(wParam) );
				m_application->pushMouseMoveEvent( 0, 0, zDelta / WHEEL_DELTA );
			}break;
		case WM_RBUTTONDBLCLK:
		case WM_LBUTTONDBLCLK:
			{
				m_isDoubleClick = true;
			}
			break;
		case WM_LBUTTONDOWN:
			{
				m_application->pushMouseButtonEvent( 0, true );
			}
			break;
		case WM_LBUTTONUP:
			{
				if( m_isDoubleClick == false )
				{
					m_application->pushMouseButtonEvent( 0, false );
				}

				m_isDoubleClick = false;
			}break;
		case WM_RBUTTONDOWN:
			{
				m_application->pushMouseButtonEvent( 1, true );
			}break;
		case WM_RBUTTONUP:
			{
				if( m_isDoubleClick == false )
				{
					m_application->pushMouseButtonEvent( 1, false );
				}

				m_isDoubleClick = false;
			}break;
		case WM_MBUTTONDOWN:
			{
				m_application->pushMouseButtonEvent( 2, true );
			}break;
		case WM_MBUTTONUP:
			{
				m_application->pushMouseButtonEvent( 2, false );
			}break;
		case WM_KEYDOWN:
			{
				unsigned int vkc = static_cast<unsigned int>( wParam );
				HKL  layout = ::GetKeyboardLayout(0);
				unsigned int vk = MapVirtualKeyExA( vkc, 0, layout );
				m_application->pushKeyEvent( vkc, translateVirtualKey_( vkc, vk ), true );
			}break;
		case WM_KEYUP:
			{
				unsigned int vkc = static_cast<unsigned int>( wParam );
				HKL  layout = ::GetKeyboardLayout(0);
				unsigned int vk = MapVirtualKeyExA( vkc, 0, layout );
				m_application->pushKeyEvent( vkc, 0, false );
			}break;
		case WM_DESTROY: 

			// Post the WM_QUIT message to 
			// quit the application terminate. 

			PostQuitMessage(0); 
			return 0;

		}

		return WindowsLayer::defWindowProc( hWnd, uMsg, wParam, lParam );
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
	void WinApplication::showMessageBox( const String& _message, const String& _header, unsigned int _style )
	{
		WindowsLayer::messageBox( m_hWnd, _message, _header, MB_ICONERROR | MB_OK );
	}
	//////////////////////////////////////////////////////////////////////////
	String WinApplication::ansiToUtf8( const String& _ansi )
	{
		Menge::String utf8;
		WindowsLayer::ansiToUtf8( _ansi, &utf8 );
		return utf8;
	}
	//////////////////////////////////////////////////////////////////////////
	String WinApplication::utf8ToAnsi( const String& _utf8 )
	{
		Menge::String ansi;
		WindowsLayer::utf8ToAnsi( _utf8, &ansi );
		return ansi;
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
	unsigned int WinApplication::translateVirtualKey_( unsigned int _vkc, unsigned int _vk )
	{
		if( _vk == 0 )
		{
			return 0;
		}

		BYTE keyState[256];
		HKL  layout = ::GetKeyboardLayout(0);
		if( ::GetKeyboardState( keyState ) == 0 )
		{
			return 0;
		}


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
		Menge::String fileName = "";
		Menge::String extention = "";
		WindowsLayer::getModuleFileName(NULL, &fileName);
		if( fileName.length() < 4 )
		{
			return false;
		}

		extention = fileName.substr(fileName.length()-4);

		std::transform( extention.begin(), extention.end(), extention.begin(), std::ptr_fun( ::tolower ) );
		/*for( int i=0; i<extention.length(); ++i )
		{
			extention[i] = tolower(extention[i]);
		}*/

		if( extention == ".scr" )
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
			String screensaverName = m_application->getScreensaverName();

			String fullModuleName = "";
			WindowsLayer::getModuleFileName(NULL, &fullModuleName);

			size_t separatorPos = fullModuleName.find_last_of('\\');
			String binFolderPath = fullModuleName.substr(0, separatorPos);
			String fullScreensaverPath = binFolderPath + "\\" + screensaverName;

			String fullScreensaverPathShort;
			WindowsLayer::getShortPathName( fullScreensaverPath, &fullScreensaverPathShort );
			WindowsLayer::setRegistryValue( HKEY_CURRENT_USER, "Control Panel\\Desktop", "SCRNSAVE.EXE", REG_SZ, reinterpret_cast<const BYTE*>( fullScreensaverPathShort.c_str() ), fullScreensaverPathShort.length()+1 );
		}
		else
		{
			WindowsLayer::deleteRegistryValue( HKEY_CURRENT_USER, "Control Panel\\Desktop", "SCRNSAVE.EXE" );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
