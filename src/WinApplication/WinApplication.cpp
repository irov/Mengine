#	include "Config/Config.h"

#	include "WinApplication.h"
#	include "Menge/Application.h"
#	include "SystemDLL.h"
#	include "LoggerConsole.h"
#	include "Interface/LogSystemInterface.h"

#	include <strsafe.h>
#	include <cstdio>

#	include "resource.h"
#	include "Menge/Utils.h"

const Menge::TCharA * config_file = "application.xml";

#	ifndef MENGE_MASTER_RELEASE
#		define LOG( message )\
		if( m_logSystem ) m_logSystem->logMessage( message + StringA("\n"), LM_LOG );
#	else
#		define LOG( message )
#	endif

#	define LOG_ERROR( message )\
	if( m_logSystem ) m_logSystem->logMessage( message + StringA("\n"), LM_ERROR );

//////////////////////////////////////////////////////////////////////////
static LONG WINAPI s_exceptionHandler(EXCEPTION_POINTERS* pExceptionPointers)
{
	EXCEPTION_RECORD* pRecord = pExceptionPointers->ExceptionRecord;
	CONTEXT* pContext = pExceptionPointers->ContextRecord;

	HANDLE hFile = ::CreateFile( MENGE_TEXT( "Bin\\Menge.log" ), GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_WRITE|FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0 );

	if( hFile != INVALID_HANDLE_VALUE )
	{
		DWORD wr;
		SYSTEMTIME tm;
		GetLocalTime(&tm);

		OSVERSIONINFO os_ver;
		os_ver.dwOSVersionInfoSize = sizeof(os_ver);
		GetVersionEx(&os_ver);

		char wBuffer[4096];
		::SetFilePointer( hFile, 0, 0, FILE_END );
		strcpy( wBuffer, "\n=============Unhandled Exception Caugth=============\n" );
		::WriteFile( hFile, wBuffer, strlen( wBuffer ),&wr, 0 );
		StringCchPrintfA( wBuffer, 4096, "Date: %02d.%02d.%d, %02d:%02d:%02d", tm.wDay, tm.wMonth, tm.wYear, tm.wHour, tm.wMinute, tm.wSecond );
		::WriteFile( hFile, wBuffer, strlen( wBuffer ),&wr, 0 );
		StringCchPrintfA( wBuffer, 4096, "OS: Windows %ld.%ld.%ld", os_ver.dwMajorVersion, os_ver.dwMinorVersion, os_ver.dwBuildNumber );
		::WriteFile( hFile, wBuffer, strlen( wBuffer ),&wr, 0 );
		strcpy( wBuffer, "\nCrash Info:\n" );
		::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
		StringCchPrintfA( wBuffer, 4096, "Exception Code: 0x%08x\n", pRecord->ExceptionCode );
		::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
		StringCchPrintfA( wBuffer, 4096, "Flags: 0x%08x\n", pRecord->ExceptionFlags );
		::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
		StringCchPrintfA( wBuffer, 4096, "Address: 0x%08x\n\n", pRecord->ExceptionAddress );
		::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
		if( ( pContext->ContextFlags & CONTEXT_INTEGER ) != 0 )
		{
			StringCchPrintfA( wBuffer, 4096, "Edi: 0x%08x\t Esi: 0x%08x\n", pContext->Edi, pContext->Esi );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			StringCchPrintfA( wBuffer, 4096, "Ebx: 0x%08x\t Edx: 0x%08x\n", pContext->Ebx, pContext->Edx );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			StringCchPrintfA( wBuffer, 4096, "Ecx: 0x%08x\t Eax: 0x%08x\n\n", pContext->Ecx, pContext->Eax );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
		}
		if( ( pContext->ContextFlags & CONTEXT_CONTROL ) != 0 )
		{
			StringCchPrintfA( wBuffer, 4096, "Ebp: 0x%08x\t Eip: 0x%08x\n", pContext->Ebp, pContext->Eip );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			StringCchPrintfA( wBuffer, 4096, "SegCs: 0x%08x\t EFlags: 0x%08x\n", pContext->SegCs, pContext->EFlags );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			StringCchPrintfA( wBuffer, 4096, "Esp: 0x%08x\t SegSs: 0x%08x\n", pContext->Esp, pContext->SegSs );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
		}
		/*switch (pRecord->ExceptionCode) 
		{
			case EXCEPTION_ACCESS_VIOLATION:
			case EXCEPTION_IN_PAGE_ERROR:
			case EXCEPTION_INT_DIVIDE_BY_ZERO:
			case EXCEPTION_STACK_OVERFLOW:
		}*/
		
		::CloseHandle( hFile );
	}
	return EXCEPTION_EXECUTE_HANDLER;
}
#	include <sstream>

//////////////////////////////////////////////////////////////////////////
/*bool initInterfaceSystem( Menge::ApplicationInterface **_system )
{
	try
	{
		*_system = new Menge::WinApplication();
	}
	catch (...)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( Menge::ApplicationInterface *_system )
{
	delete static_cast<Menge::WinApplication*>( _system );
}
*/
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	WinApplication::WinApplication( HINSTANCE _hInstance, const StringA& _commandLine ) 
		: m_running( true )
		, m_frameTime( 0.f )
		, m_mutex( 0 )
		, m_focus( true )
		, m_name( "Mengine" )
		, m_hWnd(0)
		, m_cursorInArea( false )
		, m_fullscreen( false )
		, m_handleMouse( true )
		, m_hInstance( _hInstance )
		, m_loggerConsole( NULL )
		, m_commandLine( _commandLine )
		, m_menge( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	WinApplication::~WinApplication()
	{
		if( m_logSystem != NULL && m_loggerConsole != NULL )
		{
			m_logSystem->unregisterLogger( m_loggerConsole );
			delete m_loggerConsole;
			m_loggerConsole = NULL;
		}

		if( m_menge != NULL )
		{
			m_menge->onDestroy();
			delete m_menge;
			m_menge = NULL;
		}

		if( m_mutex )
		{
			::CloseHandle( m_mutex );
			m_mutex = 0;
		}
	}
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
			int width = info.rcMonitor.right - info.rcMonitor.left;
			int height = info.rcMonitor.bottom - info.rcMonitor.top;
			app->setDesktopResolution( width, height );

			return FALSE;
		}
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::setDesktopResolution( std::size_t _width, std::size_t _height )
	{
		m_desktopWidth = _width;
		m_desktopHeight = _height;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::start()
	{
		::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX );
		::SetUnhandledExceptionFilter( &s_exceptionHandler );

		if( !::QueryPerformanceFrequency( &m_timerFrequency ) )
		{
			return false;
		}

		::QueryPerformanceCounter(&m_timer);

		m_menge = new Application( this );
		if( m_menge == NULL )
		{
			return false;
		}

		m_logSystem = m_menge->initializeLogSystem();

		if( m_logSystem != NULL && m_commandLine.find( "-console" ) != StringA::npos )
		{
			m_loggerConsole = new LoggerConsole();
			m_logSystem->registerLogger( m_loggerConsole );

			LOG_ERROR( "LogSystem initialized successfully" );	// log message anyway
		}

		if( m_logSystem != NULL && m_commandLine.find( "-verbose" ) != StringA::npos )
		{
			m_logSystem->setVerboseLevel( LM_MAX );

			LOG( "Verbose logging mode enabled" );
		}

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

		MEMORYSTATUSEX mem_st;
		GlobalMemoryStatusEx(&mem_st);
		std::sprintf( strbuffer, "Memory: %ldK total, %ldK free, %ldK Page file total, %ldK Page file free"
			, mem_st.ullTotalPhys/1024L
			, mem_st.ullAvailPhys/1024L
			, mem_st.ullTotalPageFile/1024L
			, mem_st.ullAvailPageFile );
		LOG( strbuffer );

		LOG( "Initializing Mengine..." );
		if( m_menge->initialize( config_file, m_commandLine.c_str(), true ) == false )
		{
			return false;
		}

		LOG( "Enumarating monitors..." );
		EnumDisplayMonitors( NULL, NULL, &s_monitorEnumProc, (LPARAM)this );

		LOG( "Creating Render Window..." );
		if( m_menge->createRenderWindow( 0 ) == false )
		{
			return false;
		}

		LOG( "Initializing Game data..." );
		if( m_menge->initGame() == false )
		{
			return false;
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::loop()
	{
		MSG  msg;
		POINT pos;
		LARGE_INTEGER time;
		::QueryPerformanceCounter(&m_lastTime);
		while( m_running )
		{
			::GetCursorPos( &pos );
			if( m_cursorInArea && m_handleMouse
				&& ( pos.x < m_wndInfo.rcClient.left 
				|| pos.x > m_wndInfo.rcClient.right 
				|| pos.y < m_wndInfo.rcClient.top
				|| pos.y > m_wndInfo.rcClient.bottom ) )
			{
				::ShowCursor( TRUE );
				m_menge->onMouseLeave();
				m_cursorInArea = false;
			}
			while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}

			::QueryPerformanceCounter(&time);
			m_frameTime = static_cast<float>( time.QuadPart - m_lastTime.QuadPart ) / m_timerFrequency.QuadPart * 1000.0f;
			m_lastTime = time;

			m_menge->onUpdate( m_frameTime );

			if( m_focus == false )
			{
				::Sleep( 100 );
			}
			else
			{
				::Sleep(1);
			}
		}

		// Clean up
		if( m_hWnd )
		{
			::DestroyWindow( m_hWnd );
			m_hWnd = NULL;
		}
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
	void WinApplication::createWindow(WindowHandle _handle)
	{
		::GetWindowInfo( m_hWnd, &m_wndInfo);
		//return static_cast<WINDOW_HANDLE>( m_hWnd ); 
	}
	//////////////////////////////////////////////////////////////////////////
	WindowHandle WinApplication::createWindow( const Menge::String & _name, std::size_t _width, std::size_t _height, bool _fullscreen )
	{
		DWORD dwStyle = WS_VISIBLE | WS_CLIPCHILDREN;
		RECT rc;

		m_windowWidth = _width;
		m_windowHeight = _height;

		if (!_fullscreen)
		{
			dwStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION |	WS_SYSMENU | WS_MINIMIZEBOX;
		}
		else
		{
			dwStyle |= WS_POPUP;
		}
		// Calculate window dimensions required
		// to get the requested client area
		SetRect(&rc, 0, 0, (int)m_windowWidth, (int)m_windowHeight);
		AdjustWindowRect(&rc, dwStyle, false);
		LONG width = rc.right - rc.left;
		LONG height = rc.bottom - rc.top;

		int screenw = ::GetSystemMetrics(SM_CXSCREEN);
		int screenh = ::GetSystemMetrics(SM_CYSCREEN);
		if ( width > screenw )
			width = screenw;
		if ( height > screenh )
			height = screenh;
		LONG left = (screenw - width) / 2;
		LONG top = (screenh - height) / 2;

		if( _fullscreen )
		{
			left = 0;
			top = 0;
		}

		m_name = _name;

		StringW nameW = Utils::AToW( m_name );
		m_mutex = ::CreateMutex( NULL, FALSE, nameW.c_str() );
		DWORD error = ::GetLastError();

		if( error == ERROR_ALREADY_EXISTS )
		{
			Menge::String message = Menge::String( "Another instance of " ) + m_name + Menge::String( " is already running" );
			StringW messageW = Utils::AToW( message );
			::MessageBox( NULL, messageW.c_str(), nameW.c_str(), MB_ICONWARNING );
			return false;
		}

		// Register the window class
		WNDCLASS wc;
		ZeroMemory( &wc, sizeof(WNDCLASS) );
		wc.lpfnWndProc = s_wndProc;
		wc.hInstance = m_hInstance;
		wc.hIcon = LoadIcon( m_hInstance, MAKEINTRESOURCE(IDI_MENGE) );
		wc.hCursor = LoadCursor( NULL, IDC_ARROW );
		wc.lpszClassName = (LPCWSTR)L"MengeWnd";
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		::RegisterClass( &wc );

		// Create our main window
		// Pass pointer to self
		m_hWnd = ::CreateWindow( L"MengeWnd", nameW.c_str(), dwStyle,
			left, top, width, height, NULL, 0, m_hInstance, (LPVOID)this);
		

		::GetWindowInfo( m_hWnd, &m_wndInfo);
		return static_cast<WindowHandle>( m_hWnd ); 
	}
	//////////////////////////////////////////////////////////////////////////
	LRESULT CALLBACK WinApplication::wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		switch( uMsg )
		{
		case WM_ACTIVATE:
			{
				//::GetWindowInfo( m_hWnd, &m_wndInfo);
				m_focus = (LOWORD(wParam) != WA_INACTIVE);
				m_menge->onFocus( m_focus );
				break;
			}
		case WM_PAINT:
			{
				m_menge->onPaint();
			}
			break;
		case WM_SYSKEYDOWN:
			switch( wParam )
			{
			case VK_CONTROL:
			case VK_SHIFT:
			case VK_MENU: //ALT
				//return zero to bypass defProc and signal we processed the message
				return 0;
			}
			break;
		case WM_SYSKEYUP:
			switch( wParam )
			{
			case VK_CONTROL:
			case VK_SHIFT:
			case VK_MENU: //ALT
			case VK_F10:
				//return zero to bypass defProc and signal we processed the message
				return 0;
			}
			break;
		case WM_SYSCHAR:
			// return zero to bypass defProc and signal we processed the message, unless it's an ALT-space
			if (wParam != VK_SPACE)
				return 0;
			break;
		case WM_ENTERSIZEMOVE:

			break;
		case WM_EXITSIZEMOVE:

			break;
		case WM_MOVE:
			if( m_hWnd != 0 )
			{
				::GetWindowInfo( m_hWnd, &m_wndInfo);
				m_menge->onWindowMovedOrResized();
			}
			break;
		case WM_DISPLAYCHANGE:
			m_menge->onWindowMovedOrResized();

			break;
		case WM_SIZE:
			if( m_hWnd != 0)
			{
				::GetWindowInfo( m_hWnd, &m_wndInfo);
				m_menge->onWindowMovedOrResized();
			}
			break;
		case WM_GETMINMAXINFO:
			// Prevent the window from going smaller than some minimu size
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
			break;
		case WM_CLOSE:
			m_menge->onClose();
			break;
		case WM_MOUSEMOVE:
			if( m_handleMouse )
			{
				if( !m_cursorInArea )
				{
					m_cursorInArea = true;
					::ShowCursor( FALSE );
					m_menge->onMouseEnter();
				}
				m_menge->onMouseMove( (float)(short)LOWORD(lParam), (float)(short)HIWORD(lParam), 0 );
			}
			break;
		case WM_LBUTTONDOWN:
			m_menge->onMouseButtonEvent( 0, true );
			break;
		case WM_LBUTTONUP:
			m_menge->onMouseButtonEvent( 0, false );
			break;
		case WM_RBUTTONDOWN:
			m_menge->onMouseButtonEvent( 1, true );
			break;
		case WM_RBUTTONUP:
			m_menge->onMouseButtonEvent( 1, false );
			break;
		case WM_MBUTTONDOWN:
			m_menge->onMouseButtonEvent( 2, true );
			break;
		case WM_MBUTTONUP:
			m_menge->onMouseButtonEvent( 2, false );
			break;
		}
		return ::DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t WinApplication::getDesktopWidth() const
	{
		return m_desktopWidth;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t WinApplication::getDesktopHeight() const
	{
		return m_desktopHeight;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::minimizeWindow()
	{
		::ShowWindow( m_hWnd, SW_MINIMIZE );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyWindowModeChanged( std::size_t _width, std::size_t _height, bool _fullscreen )
	{
		m_windowWidth = _width;
		m_windowHeight = _height;

		m_fullscreen = _fullscreen;

		DWORD dwStyle = WS_VISIBLE | WS_CLIPCHILDREN;
		if( !_fullscreen )
		{
			dwStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION |	WS_SYSMENU | WS_MINIMIZEBOX;

			// When switching back to windowed mode, need to reset window size 
			// after device has been restored
			RECT rc;
			SetRect(&rc, 0, 0, (LONG)_width, (LONG)_height);
			//AdjustWindowRect(&rc, GetWindowLong(m_hWnd, GWL_STYLE), false);
			AdjustWindowRect(&rc, dwStyle, false);
			int winWidth = rc.right - rc.left;
			int winHeight = rc.bottom - rc.top;
			int screenw = GetSystemMetrics(SM_CXSCREEN);
			int screenh = GetSystemMetrics(SM_CYSCREEN);
			int left = (screenw - winWidth) / 2;
			int top = (screenh - winHeight) / 2;

			SetWindowLong( m_hWnd, GWL_STYLE, dwStyle );
			SetWindowPos(m_hWnd, HWND_NOTOPMOST, left, top, winWidth, winHeight,
				SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOACTIVATE);

		}
		else
		{
			dwStyle |= WS_POPUP;
			SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
			SetWindowPos( m_hWnd, NULL, 0, 0, (LONG)_width, (LONG)_height, SWP_NOACTIVATE );
		}
		::ShowWindow( m_hWnd, SW_NORMAL );
		//::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);


		//m_listener->onWindowMovedOrResized();
	}
	//////////////////////////////////////////////////////////////////////////
	SystemDLLInterface* WinApplication::loadSystemDLL( const String& _dll )
	{
		return new WinSystemDLL( _dll );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::unloadSystemDLL( SystemDLLInterface* _interface )
	{
		delete static_cast<WinSystemDLL*>( _interface );
	}
	//////////////////////////////////////////////////////////////////////////
	float WinApplication::getDeltaTime()
	{
		LARGE_INTEGER time;
		::QueryPerformanceCounter( &time );
		float deltaTime = static_cast<float>( time.QuadPart - m_timer.QuadPart )  / m_timerFrequency.QuadPart * 1000.0f;
		m_timer = time;

		return deltaTime;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::setHandleMouse( bool _handle )
	{
		m_handleMouse = _handle;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::showMessageBox( const String& _message, const String& _header, unsigned int _style )
	{
		StringW message_w = Utils::AToW( _message );
		StringW header_w = Utils::AToW( _header );
		::MessageBox( m_hWnd, message_w.c_str(), header_w.c_str(), MB_ICONERROR | MB_OK );
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge

