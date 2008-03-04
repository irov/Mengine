#	include "WinApplication.h"

//#	include "Menge/Application.h"

//#	include "System/OgreInputSystem/OgreInputSystemInterface.h"
//#	include "System/OgreRenderSystem/OgreRenderSystemInterface.h"

//#	include "Interface/LogSystemInterface.h"
//#	include "Interface/SoundSystemInterface.h"
//#	include "Interface/ParticleSystemInterface.h"
#	include "Interface/FileSystemInterface.h"


//#	include "OIS/OIS.h"

#	include "SystemDLL.h"

//#	include <map>

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( ApplicationInterface** _ptrInterface )
{
	try
	{
		*_ptrInterface = new WinApplication();
	}
	catch (...)
	{
		return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
void releaseInterfaceSystem( ApplicationInterface* _ptrInterface )
{
	delete static_cast<WinApplication*>(_ptrInterface);
}
//////////////////////////////////////////////////////////////////////////
WinApplication::WinApplication() 
: m_running( true )
, m_frameTime( 0.f )
, m_mutex(0)
, m_focus( true )
, m_fileSystem( 0 )
, m_name("Menge-engine")
, m_fileSystemDLL( 0 )
, m_listener(0)
, m_hWnd(0)
, m_cursorInArea(false)
, m_active(false)
, m_primaryMonitorAspect(4.f/3.f)
{}
//////////////////////////////////////////////////////////////////////////
WinApplication::~WinApplication()
{
	if( m_listener )
	{
		m_listener->onDestroy();
	}

	//m_application->finalize();
	unloadSystemDLL( m_fileSystemDLL );

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
		float primaryMonitorAspect = static_cast<float>( width ) / static_cast<float> ( height );
		app->setPrimaryMonitorAspect( primaryMonitorAspect );
		
		return FALSE;
	}
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
void WinApplication::setPrimaryMonitorAspect( float _aspect )
{
	m_primaryMonitorAspect = _aspect;
}
//////////////////////////////////////////////////////////////////////////
bool WinApplication::init( const char* _name, ApplicationListenerInterface* _listener )
{
	if( !_listener )
	{
		return false;
	}
	m_listener = _listener;

	if( !::QueryPerformanceFrequency( &m_timerFrequency ) )
	{
		return false;
	}

	m_name.assign( _name );
	//printf("use file system [%s]\n", DllFileSystem.c_str() );
#ifdef _DEBUG
	std::string DllModuleSetting = "DllModuleDebug";
	std::string DllFileSystem = "Systems/OgreFileSystem_d.dll";
#else
	std::string DllModuleSetting = "DllModuleRelease";
	std::string DllFileSystem = "Systems/OgreFileSystem.dll";
#endif

	m_fileSystemDLL = loadSystemDLL( DllFileSystem.c_str() );
	m_fileSystem = m_fileSystemDLL->getInterface<FileSystemInterface>();

	m_fileSystem->loadPath(".");

	EnumDisplayMonitors( NULL, NULL, &s_monitorEnumProc, (LPARAM)this );

	return true;
}
//////////////////////////////////////////////////////////////////////////
void WinApplication::run()
{
	MSG  msg;
	static bool resetTime = false;
	POINT pos;
	LARGE_INTEGER time;
	::QueryPerformanceCounter(&m_lastTime);
	while( m_running )
	{
		::GetCursorPos( &pos );
		if( m_cursorInArea 
			&& ( pos.x < m_wndInfo.rcClient.left 
			|| pos.x > m_wndInfo.rcClient.right 
			|| pos.y < m_wndInfo.rcClient.top
			|| pos.y > m_wndInfo.rcClient.bottom ) )
		{
			::ShowCursor( TRUE );
			m_listener->onMouseLeave();
			m_cursorInArea = false;
		}
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
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
				resetTime = true;
			}
			m_listener->onUpdate( m_frameTime );
			::QueryPerformanceCounter(&time);
			m_frameTime = static_cast<float>( time.QuadPart - m_lastTime.QuadPart ) / m_timerFrequency.QuadPart * 1000.0f;
			m_lastTime = time;

			if ( resetTime )
			{
				m_frameTime = 0.0f;
				resetTime = false;
			}
		}
		else if( !m_active && m_focus )
		{
			m_focus = false;
			m_listener->onFocus( false );
		}

		::Sleep(1);
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
WINDOW_HANDLE WinApplication::createWindow( const char* _name, unsigned int _width, unsigned int _height, bool _fullscreen )
{
	DWORD dwStyle = WS_VISIBLE | WS_CLIPCHILDREN;
	RECT rc;

	int width = _width;
	int height = _height;
	int left;
	int top;

	if (!_fullscreen)
	{
		dwStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION |	WS_SYSMENU | WS_MINIMIZEBOX;
	}
	else
	{
		dwStyle |= WS_POPUP;
		top = left = 0;
	}
	// Calculate window dimensions required
	// to get the requested client area
	SetRect(&rc, 0, 0, width, height);
	AdjustWindowRect(&rc, dwStyle, false);
	width = rc.right - rc.left;
	height = rc.bottom - rc.top;

	int screenw = ::GetSystemMetrics(SM_CXSCREEN);
	int screenh = ::GetSystemMetrics(SM_CYSCREEN);
	if ( width > screenw )
		width = screenw;
	if ( height > screenh )
		height = screenh;
	left = (screenw - width) / 2;
	top = (screenh - height) / 2;


	/// patch for ansi names
	char *ansistr = NULL;
	int length = MultiByteToWideChar(CP_UTF8, 0, _name, strlen( _name ), NULL, NULL );
	WCHAR *lpszW = NULL;

	lpszW = new WCHAR[length+1];
	ansistr = ( char * ) calloc ( sizeof(char), length+5 );

	//this step intended only to use WideCharToMultiByte
	MultiByteToWideChar(CP_UTF8, 0, _name, -1, lpszW, length );

	//Conversion to ANSI (CP_ACP)
	WideCharToMultiByte(CP_ACP, 0, lpszW, -1, ansistr, length, NULL, NULL);

	ansistr[length] = 0;

	delete[] lpszW;
	////

	m_name.assign( ansistr );

	m_mutex = ::CreateMutexA( NULL, FALSE, _name );
	DWORD error = ::GetLastError();

	if( error == ERROR_ALREADY_EXISTS )
	{
		std::string message = std::string("Another instance of ") + std::string( ansistr ) + std::string(" is already running");
		::MessageBoxA( NULL, message.c_str(), ansistr, MB_ICONWARNING );
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
	LoadIcon(0, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
	(HBRUSH)GetStockObject(BLACK_BRUSH), 0, "MengeWnd" 
	};
	::RegisterClass(&wc);

	// Create our main window
	// Pass pointer to self

	CREATESTRUCT createStruct;
	createStruct.lpCreateParams = (LPVOID)666;

	m_hWnd = ::CreateWindow("MengeWnd", ansistr, dwStyle,
		left, top, width, height, NULL, 0, hInstance, (LPVOID)this);

	::GetWindowInfo( m_hWnd, &m_wndInfo);
	return static_cast<WINDOW_HANDLE>( m_hWnd ); 
}
//////////////////////////////////////////////////////////////////////////
FileSystemInterface* WinApplication::getFileSystemInterface()
{
	return m_fileSystem;
}
//////////////////////////////////////////////////////////////////////////
SystemDLLInterface* WinApplication::loadSystemDLL( const char* _dll )
{
	return new WinSystemDLL( _dll );
}
//////////////////////////////////////////////////////////////////////////
void WinApplication::unloadSystemDLL( SystemDLLInterface* _interface )
{
	delete static_cast<WinSystemDLL*>( _interface );
}
//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WinApplication::wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_ACTIVATE:
		{
			::GetWindowInfo( m_hWnd, &m_wndInfo);
			m_active = (LOWORD(wParam) != WA_INACTIVE);
			/*if(m_renderWindow)
				m_renderWindow->setActive( active );*/
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
		::GetWindowInfo( m_hWnd, &m_wndInfo);
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

		//m_renderWindow->destroy();
		m_listener->onClose();
		break;
	case WM_MOUSEMOVE:
		if( !m_cursorInArea )
		{
			m_cursorInArea = true;
			::ShowCursor( FALSE );
			m_listener->onMouseEnter();
		}
		m_listener->onMouseMove( (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), 0 );
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
float WinApplication::getMonitorAspectRatio()
{
	return m_primaryMonitorAspect;	
}
//////////////////////////////////////////////////////////////////////////
void WinApplication::minimizeWindow()
{
	::ShowWindow( m_hWnd, SW_MINIMIZE );
}
