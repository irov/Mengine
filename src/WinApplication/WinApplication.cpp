#	include "WinApplication.h"

#	include "SystemDLL.h"

#	include "Config/Config.h"

#	include "Game/resource.h"

#if	SAVE_DUMP == 1
#include <dbghelp.h>
#include <shellapi.h>
#include <shlobj.h>
#include <strsafe.h>

#pragma  comment( lib, "dbghelp.lib")

//////////////////////////////////////////////////////////////////////////
static LONG WINAPI s_generateDump(EXCEPTION_POINTERS* pExceptionPointers)
{
	BOOL bMiniDumpSuccessful;
	WCHAR szPath[MAX_PATH]; 
	WCHAR szFileName[MAX_PATH]; 
	WCHAR* szAppName = L"Mengine_Dump";
	WCHAR* szVersion = L"v1.0";
	DWORD dwBufferSize = MAX_PATH;
	HANDLE hDumpFile;
	SYSTEMTIME stLocalTime;
	MINIDUMP_EXCEPTION_INFORMATION ExpParam;

	GetLocalTime( &stLocalTime );
	GetTempPathW( dwBufferSize, szPath );

	StringCchPrintfW( szFileName, MAX_PATH, L"%s%s", szPath, szAppName );
	CreateDirectoryW( szFileName, NULL );

	StringCchPrintfW( szFileName, MAX_PATH, L"%s%s\\%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp", 
		szPath, szAppName, szVersion, 
		stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay, 
		stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond, 
		GetCurrentProcessId(), GetCurrentThreadId());
	hDumpFile = CreateFileW(szFileName, GENERIC_READ|GENERIC_WRITE, 
		FILE_SHARE_WRITE|FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

	ExpParam.ThreadId = GetCurrentThreadId();
	ExpParam.ExceptionPointers = pExceptionPointers;
	ExpParam.ClientPointers = TRUE;

	bMiniDumpSuccessful = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), 
		hDumpFile, MiniDumpWithDataSegs, &ExpParam, NULL, NULL);

	return EXCEPTION_EXECUTE_HANDLER;

}
#endif
#	include <sstream>
//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( Menge::ApplicationInterface** _ptrInterface )
{
	try
	{
		*_ptrInterface = new Menge::WinApplication();
	
		std::ostringstream str;
		str << ((int)_ptrInterface);
		
		//::MessageBoxA( NULL, str.str().c_str(), "name", MB_ICONWARNING );
	}
	catch (...)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( Menge::ApplicationInterface* _ptrInterface )
{
	delete static_cast<Menge::WinApplication*>(_ptrInterface);
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	WinApplication::WinApplication() 
		: m_running( true )
		, m_frameTime( 0.f )
		, m_mutex(0)
		, m_focus( true )
		, m_name("Mengine")
		, m_listener(0)
		, m_hWnd(0)
		, m_cursorInArea(false)
		, m_active(false)
		//, m_primaryMonitorAspect(4.f/3.f)
		, m_desktopWidht( 800 )
		, m_desktopHeight( 600 )
		, m_winWidth( 800 )
		, m_winHeight( 600 )
		, m_fullscreen( false )
		, m_handleMouse( true )
	{}
	//////////////////////////////////////////////////////////////////////////
	WinApplication::~WinApplication()
	{
		if( m_listener )
		{
			m_listener->onDestroy();
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
			//float primaryMonitorAspect = static_cast<float>( width ) / static_cast<float> ( height );
			//app->setPrimaryMonitorAspect( primaryMonitorAspect );
			app->setDesktopResolution( width, height );

			return FALSE;
		}
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	/*void WinApplication::setPrimaryMonitorAspect( float _aspect )
	{
	m_primaryMonitorAspect = _aspect;
	}*/
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::setDesktopResolution( int _width, int _height )
	{
		m_desktopWidht = _width;
		m_desktopHeight = _height;
	}
	//////////////////////////////////////////////////////////////////////////
	bool WinApplication::init( const String & _name, ApplicationListenerInterface* _listener )
	{

#if SAVE_DUMP == 1
		::SetUnhandledExceptionFilter( &s_generateDump );
#endif

		if( !_listener )
		{
			return false;
		}

		m_listener = _listener;

		if( !::QueryPerformanceFrequency( &m_timerFrequency ) )
		{
			return false;
		}

		::QueryPerformanceCounter(&m_timer);

		m_name.assign( _name );

		//initInterfaceSystem( &m_fileSystem );

		//m_fileSystem->loadPath(".");

		EnumDisplayMonitors( NULL, NULL, &s_monitorEnumProc, (LPARAM)this );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::run()
	{
		MSG  msg;
		//static bool resetTime = false;
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
				m_listener->onMouseLeave();
				m_cursorInArea = false;
			}
			while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			//m_running = m_renderWindow->isClosed() == false;

			if( m_active )
			{
				if( !m_focus )
				{
					m_focus = true;
					m_listener->onFocus( true );
					//resetTime = true;
				}
				m_listener->onUpdate( m_frameTime );
				::QueryPerformanceCounter(&time);
				m_frameTime = static_cast<float>( time.QuadPart - m_lastTime.QuadPart ) / m_timerFrequency.QuadPart * 1000.0f;
				/*if( m_frameTime > MAX_UPDATE_TIMING )
				{
				m_frameTime = MAX_UPDATE_TIMING;
				}*/
				m_lastTime = time;

				/*if ( resetTime )
				{
				m_frameTime = 0.0f;
				resetTime = false;
				}
				printf("timing %.2f\n", m_frameTime );*/
			}
			else if( !m_active && m_focus )
			{
				m_focus = false;
				m_listener->onFocus( false );
			}

			::Sleep(1);
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
	WindowHandle WinApplication::createWindow( Menge::String _name, float _width, float _height, bool _fullscreen )
	{
		DWORD dwStyle = WS_VISIBLE | WS_CLIPCHILDREN;
		RECT rc;

		m_winWidth = _width;
		m_winHeight = _height;

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
		SetRect(&rc, 0, 0, (int)m_winWidth, (int)m_winHeight);
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

		/// patch for ansi names
		char *ansistr = NULL;
		int length = MultiByteToWideChar(CP_UTF8, 0, _name.c_str(), _name.size(), NULL, NULL );
		WCHAR *lpszW = NULL;

		lpszW = new WCHAR[length+1];
		ansistr = ( char * ) calloc ( sizeof(char), length+5 );

		//this step intended only to use WideCharToMultiByte
		MultiByteToWideChar(CP_UTF8, 0, _name.c_str(), -1, lpszW, length );

		//Conversion to ANSI (CP_ACP)
		WideCharToMultiByte(CP_ACP, 0, lpszW, -1, ansistr, length, NULL, NULL);

		
		ansistr[length] = 0;

		delete[] lpszW;
		////
		m_name.assign( ansistr );
		
		free( ansistr );

		m_mutex = ::CreateMutexA( NULL, FALSE, m_name.c_str() );
		DWORD error = ::GetLastError();

		if( error == ERROR_ALREADY_EXISTS )
		{
			std::string message = std::string("Another instance of ") + m_name + std::string(" is already running");
			::MessageBoxA( NULL, message.c_str(), m_name.c_str(), MB_ICONWARNING );
			return false;
		}

		HINSTANCE hInstance = ::GetModuleHandle( NULL );
		// Register the window class
		// NB allow 4 bytes of window data for D3D9RenderWindow pointer
		WNDCLASS wc = 
		{ 0,
		s_wndProc,
		0, 
		0, 
		hInstance,
		LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BUTTONS)), LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)GetStockObject(BLACK_BRUSH), 0, "MengeWnd" 
		};
		/*ZeroMemory( &wc, sizeof(WNDCLASSEX) );
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.lpfnWndProc = s_wndProc;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BUTTONS));
		wc.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BUTTONS));
		wc.lpszClassName = (LPCSTR)"MengeWnd";
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);*/
		::RegisterClass(&wc);

		// Create our main window
		// Pass pointer to self

		//CREATESTRUCT createStruct;
		//createStruct.lpCreateParams = (LPVOID)666;

		m_hWnd = ::CreateWindow("MengeWnd", m_name.c_str(), dwStyle,
			left, top, width, height, NULL, 0, hInstance, (LPVOID)this);
	
		::GetWindowInfo( m_hWnd, &m_wndInfo);
		return static_cast<WINDOW_HANDLE>( m_hWnd ); 
	}
	//////////////////////////////////////////////////////////////////////////
	LRESULT CALLBACK WinApplication::wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		switch( uMsg )
		{
		case WM_ACTIVATE:
			{
				//::GetWindowInfo( m_hWnd, &m_wndInfo);
				m_active = (LOWORD(wParam) != WA_INACTIVE);
				m_listener->onActive( m_active );
				break;
			}
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

			::GetWindowInfo( m_hWnd, &m_wndInfo);
			m_listener->onWindowMovedOrResized();
			break;
		case WM_DISPLAYCHANGE:
			m_listener->onWindowMovedOrResized();

			break;
		case WM_SIZE:
			::GetWindowInfo( m_hWnd, &m_wndInfo);
			m_listener->onWindowMovedOrResized();
			break;
		case WM_GETMINMAXINFO:
			// Prevent the window from going smaller than some minimu size
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
			break;
		case WM_CLOSE:


			m_listener->onClose();
			break;
		case WM_MOUSEMOVE:
			if( m_handleMouse )
			{
				if( !m_cursorInArea )
				{
					m_cursorInArea = true;
					::ShowCursor( FALSE );
					m_listener->onMouseEnter();
				}
				m_listener->onMouseMove( (float)(short)LOWORD(lParam), (float)(short)HIWORD(lParam), 0 );
			}
			break;
		case WM_LBUTTONDOWN:
			m_listener->onMouseButtonEvent( 0, true );
			break;
		case WM_LBUTTONUP:
			m_listener->onMouseButtonEvent( 0, false );
			break;
		case WM_RBUTTONDOWN:
			m_listener->onMouseButtonEvent( 1, true );
			break;
		case WM_RBUTTONUP:
			m_listener->onMouseButtonEvent( 1, false );
			break;
		case WM_MBUTTONDOWN:
			m_listener->onMouseButtonEvent( 2, true );
			break;
		case WM_MBUTTONUP:
			m_listener->onMouseButtonEvent( 2, false );
			break;
		}
		return ::DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
	//////////////////////////////////////////////////////////////////////////
	/*float WinApplication::getMonitorAspectRatio()
	{
	return m_primaryMonitorAspect;	
	}*/
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::getDesktopResolution( int* _width, int* _heigth )
	{
		*_width = m_desktopWidht;
		*_heigth = m_desktopHeight;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::minimizeWindow()
	{
		::ShowWindow( m_hWnd, SW_MINIMIZE );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::notifyWindowModeChanged( std::size_t _width, std::size_t _height, bool _fullscreen )
	{
		m_winWidth = _width;
		m_winHeight = _height;
		m_fullscreen = _fullscreen;

		DWORD dwStyle = WS_VISIBLE | WS_CLIPCHILDREN;
		if( !_fullscreen )
		{
			dwStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION |	WS_SYSMENU | WS_MINIMIZEBOX;

			// When switching back to windowed mode, need to reset window size 
			// after device has been restored
			RECT rc;
			SetRect(&rc, 0, 0, _width, _height);
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
			SetWindowPos( m_hWnd, NULL, 0, 0, _width, _height, SWP_NOACTIVATE );
		}
		::ShowWindow( m_hWnd, SW_NORMAL );
		//::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);


		//m_listener->onWindowMovedOrResized();
	}
	//////////////////////////////////////////////////////////////////////////
	SystemDLLInterface* WinApplication::loadSystemDLL( const String & _dll )
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
		::MessageBox( m_hWnd, _message.c_str(), _header.c_str(), MB_ICONERROR | MB_OK );
	}
	//////////////////////////////////////////////////////////////////////////
}