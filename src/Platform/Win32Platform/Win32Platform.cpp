#	include "Win32Platform.h"

#	include "Interface/WindowsLayerInterface.h"
#	include "Interface/OptionsInterface.h"
#	include "Interface/ApplicationInterface.h"
#	include "Interface/InputSystemInterface.h"
#	include "Interface/ConfigInterface.h"
#	include "Interface/StringizeInterface.h"
#	include "Interface/UnicodeInterface.h"
#	include "Interface/TimerInterface.h"
#	include "Interface/FileSystemInterface.h"

#	include "Logger/Logger.h"

#	include <cstdio>
#	include <clocale>

#   include <errno.h>

#	include <ctime>
#	include <algorithm>
#	include <functional>

#	include <sstream>

#   include <WinBase.h>
#   include <Psapi.h>
#	include <tlhelp32.h>
#   include <Shlwapi.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( Platform, Menge::Win32Platform );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	static const float s_activeFrameTime = 1000.f / 60.f;
	static const float s_inactiveFrameTime = 100.f;
	//////////////////////////////////////////////////////////////////////////
#	ifndef MENGINE_WINDOW_CLASSNAME
#	define MENGINE_WINDOW_CLASSNAME (L"MengineWindow")
#	endif
	//////////////////////////////////////////////////////////////////////////
	Win32Platform::Win32Platform()
		: m_hInstance( NULL )
		, m_hWnd( NULL )
		, m_alreadyRunningMonitor( nullptr )
		, m_fpsMonitor( nullptr )
		, m_active( false )
		, m_update( false )
		, m_close( false )
		, m_vsync( false )
		, m_pauseUpdatingTime( -1.f )
		, m_cursorInArea( false )
		, m_clickOutArea( false )
		, m_isDoubleClick( false )
		, m_cursorMode( false )
		, m_cursor( nullptr )
		, m_lastMouse( false )
		, m_lastMouseX( 0 )
		, m_lastMouseY( 0 )
		, m_touchpad( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Win32Platform::~Win32Platform()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32Platform::_initialize()
	{
		// seed randomizer
		//LARGE_INTEGER randomSeed;
		//::QueryPerformanceCounter( &randomSeed );
		//srand( randomSeed.LowPart );

		SYSTEMTIME tm;
		GetLocalTime( &tm );
		LOGGER_INFO( m_serviceProvider )("Date: %02d.%02d.%d, %02d:%02d:%02d"
			, tm.wDay
			, tm.wMonth
			, tm.wYear
			, tm.wHour
			, tm.wMinute
			, tm.wSecond
			);

		MEMORYSTATUSEX mem_st;
		GlobalMemoryStatusEx( &mem_st );
		LOGGER_INFO( m_serviceProvider )("Memory: %uK total, %uK free, %uK Page file total, %uK Page file free"
			, (uint32_t)(mem_st.ullTotalPhys / 1024UL)
			, (uint32_t)(mem_st.ullAvailPhys / 1024UL)
			, (uint32_t)(mem_st.ullTotalPageFile / 1024UL)
			, (uint32_t)(mem_st.ullAvailPageFile / 1024UL)
			);

		if( WINDOWSLAYER_SERVICE(m_serviceProvider)->setProcessDPIAware() == false )
		{
			LOGGER_ERROR( m_serviceProvider )("Application not setup Process DPI Aware"
				);
		}

		m_hInstance = GetModuleHandle( NULL );

		m_cursors[STRINGIZE_STRING_LOCAL( m_serviceProvider, "IDC_ARROW" )] = LoadCursor( NULL, IDC_ARROW );
		m_cursors[STRINGIZE_STRING_LOCAL( m_serviceProvider, "IDC_UPARROW" )] = LoadCursor( NULL, IDC_UPARROW );
		m_cursors[STRINGIZE_STRING_LOCAL( m_serviceProvider, "IDC_HAND" )] = LoadCursor( NULL, IDC_HAND );
		m_cursors[STRINGIZE_STRING_LOCAL( m_serviceProvider, "IDC_HELP" )] = LoadCursor( NULL, IDC_HELP );

		m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "PC" ) );

#	ifndef _WIN64
		m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "WIN32" ) );
#	else
		m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "WIN64" ) );
#	endif

		m_touchpad = false;

		if( HAS_OPTION( m_serviceProvider, "win32" ) )
		{
			m_platformTags.clear();
			m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "PC" ) );
			m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "WIN32" ) );
			
			m_touchpad = false;
		}
		else if( HAS_OPTION( m_serviceProvider, "win64" ) )
		{
			m_platformTags.clear();
			m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "PC" ) );
			m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "WIN64" ) );

			m_touchpad = false;
		}
		else if( HAS_OPTION(m_serviceProvider, "ios") )
		{ 
			m_platformTags.clear();
			m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "MOBILE" ) );
			m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "IOS" ) );
			
			m_touchpad = true;
		}
		else if( HAS_OPTION( m_serviceProvider, "android" ) )
		{
			m_platformTags.clear();
			m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "MOBILE" ) );
			m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "ANDROID" ) );

			m_touchpad = true;
		}
		else if( HAS_OPTION( m_serviceProvider, "wp" ) )
		{
			m_platformTags.clear();
			m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "MOBILE" ) );
			m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "WP" ) );
						
			m_touchpad = true;
		}
		else if( HAS_OPTION( m_serviceProvider, "osx" ) )
		{
			m_platformTags.clear();
			m_platformTags.addTag( STRINGIZE_STRING_LOCAL( m_serviceProvider, "OSX" ) );

			m_touchpad = false;
		}

		if( HAS_OPTION( m_serviceProvider, "touchpad" ) )
		{
			m_touchpad = true;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32Platform::_finalize()
	{
        m_platformTags.clear();
        
        m_cursors.clear();

        if( m_alreadyRunningMonitor != nullptr )
        {
            m_alreadyRunningMonitor->stop();

            delete m_alreadyRunningMonitor;
            m_alreadyRunningMonitor = nullptr;
        }

        if( m_hWnd != NULL )
        {
            WINDOWSLAYER_SERVICE( m_serviceProvider )
                ->destroyWindow( m_hWnd );

            m_hWnd = NULL;
        }

        if( m_hInstance != NULL )
        {
            WINDOWSLAYER_SERVICE( m_serviceProvider )
                ->unregisterClass( MENGINE_WINDOW_CLASSNAME, m_hInstance );

            m_hInstance = NULL;
        }

        if( m_fpsMonitor != nullptr )
        {
            m_fpsMonitor->finalize();

            delete m_fpsMonitor;
            m_fpsMonitor = nullptr;
        }
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32Platform::update()
	{
		this->setActive_( true );

		TIMER_SERVICE( m_serviceProvider )
			->resetDeltaTime();

		bool nopause = HAS_OPTION( m_serviceProvider, "nopause" );

#	ifndef _DEBUG
		try
#	endif
		{
			while( m_close == false )
			{
				float frameTime = TIMER_SERVICE( m_serviceProvider )
					->getDeltaTime();

				WINDOWSLAYER_SERVICE( m_serviceProvider )
					->updateMessage( NULL );

				m_update = true;
				
				bool updating = APPLICATION_SERVICE(m_serviceProvider)
					->beginUpdate();

				if( updating == true )
				{
					if( m_pauseUpdatingTime >= 0.f )
					{
						frameTime = m_pauseUpdatingTime;
						m_pauseUpdatingTime = -1.f;
					}

					APPLICATION_SERVICE( m_serviceProvider )
						->tick( frameTime );

					if( m_fpsMonitor != nullptr )
					{
						m_fpsMonitor->monitor();
					}
					else
					{
						::Sleep( 0 );
					}
				}
				else
				{
					::Sleep( 100 );
				}				

				bool focus = APPLICATION_SERVICE( m_serviceProvider )
					->isFocus();

				if( focus == true && m_active == true || nopause == true )
				{
					if( APPLICATION_SERVICE( m_serviceProvider )->render() == true )
					{
						APPLICATION_SERVICE( m_serviceProvider )
							->flush();
					}
				}
				else
				{
					if( m_pauseUpdatingTime < 0.f )
					{
						m_pauseUpdatingTime = frameTime;
					}
				}

				APPLICATION_SERVICE( m_serviceProvider )
					->endUpdate();

				m_update = false;
			}
		}
#	ifndef _DEBUG
		catch( const std::exception & ex )
		{
			LOGGER_CRITICAL( m_serviceProvider )("Win32Platform std::exception '%s'"
				, ex.what()
				);
		}
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32Platform::stop()
	{
		m_mouseEvent.stop();

		if( m_hWnd != NULL )
		{
			WINDOWSLAYER_SERVICE( m_serviceProvider )
				->destroyWindow( m_hWnd );

			m_hWnd = NULL;
		}
	}
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::setIcon( uint32_t _icon )
    {
        m_icon = _icon;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Win32Platform::getIcon() const
    {
        return m_icon;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32Platform::setProjectTitle( const WString & _projectTitle )
    {
        m_projectTitle = _projectTitle;
    }
    //////////////////////////////////////////////////////////////////////////
    const WString & Win32Platform::getProjectTitle() const
    {
        return m_projectTitle;
    }
	//////////////////////////////////////////////////////////////////////////
	size_t Win32Platform::getShortPathName( const WString & _path, WChar * _shortpath, size_t _len ) const
	{
		DWORD len = GetShortPathName( _path.c_str(), _shortpath, (DWORD)_len );

		return (size_t)len;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32Platform::getMaxClientResolution( Resolution & _resolution ) const
	{
		RECT workArea;
		SystemParametersInfo( SPI_GETWORKAREA, 0, &workArea, 0 );

		RECT clientArea = workArea;
		::AdjustWindowRect( &clientArea, WS_OVERLAPPEDWINDOW, FALSE );
		uint32_t maxClientWidth = 2 * (workArea.right - workArea.left) - (clientArea.right - clientArea.left);
		uint32_t maxClientHeight = 2 * (workArea.bottom - workArea.top) - (clientArea.bottom - clientArea.top);

		_resolution.setWidth( maxClientWidth );
		_resolution.setHeight( maxClientHeight );
	}
	//////////////////////////////////////////////////////////////////////////
	const Tags & Win32Platform::getPlatformTags() const
	{
		return m_platformTags;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32Platform::hasTouchpad() const
	{
		return m_touchpad;
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

				Win32Platform * app = (Win32Platform *)createStruct->lpCreateParams;

				::SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)app );

				return (LRESULT)NULL;
			}
			break;
		}

		LONG_PTR value = ::GetWindowLongPtr( hWnd, GWLP_USERDATA );

		Win32Platform * platform = (Win32Platform *)value;

		if( platform == NULL )
		{
			LRESULT result = ::DefWindowProc( hWnd, uMsg, wParam, lParam );

			return result;
		}

		LRESULT app_result = platform->wndProc( hWnd, uMsg, wParam, lParam );

		return app_result;
	}
	//////////////////////////////////////////////////////////////////////////
	LRESULT Win32Platform::wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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
		case WM_ACTIVATE:
			{
				DWORD flagActive = LOWORD( wParam ); // флажок активизации
				BOOL minimized = (BOOL)HIWORD( wParam ); // флажок минимизации

				bool active = (flagActive != WA_INACTIVE) && (minimized == FALSE);

				this->setActive_( active );

				return FALSE;
			}break;
		case WM_ACTIVATEAPP:
			{
			}break;
		case WM_SETFOCUS:
			{				
			}break;
		case WM_KILLFOCUS:
			{				
			}break;
		case WM_PAINT:
			{
				if( m_update == false )
				{
					bool fullscreen = APPLICATION_SERVICE( m_serviceProvider )
						->getFullscreenMode();

					if( fullscreen == false )
					{
						APPLICATION_SERVICE( m_serviceProvider )->paint();
					}
				}
			}break;
		case WM_DISPLAYCHANGE:
			{
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
					this->setActive_( true );
				}
				else if( wParam == SIZE_MINIMIZED )
				{
					this->setActive_( false );
				}
				else if( wParam == SIZE_RESTORED /*&& m_application->getFullscreenMode() == true*/ )
				{
					bool fullsreenMode = APPLICATION_SERVICE( m_serviceProvider )
						->getFullscreenMode();

					if( fullsreenMode == true )
					{
						this->setActive_( true );
					}
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
				m_close = true;

				return FALSE;
			}break;
		case WM_SYSKEYDOWN:
			{
				unsigned int vkc = static_cast<unsigned int>(wParam);
				HKL  layout = ::GetKeyboardLayout( 0 );
				unsigned int vk = MapVirtualKeyEx( vkc, 0, layout );

				mt::vec2f point;
				this->calcCursorPosition_( point );

				WChar tvk = translateVirtualKey_( vkc, vk );

				KeyCode code = (KeyCode)vkc;

				INPUT_SERVICE( m_serviceProvider )
					->pushKeyEvent( point.x, point.y, code, tvk, true, false );
			}break;
		case WM_SYSKEYUP:
			{
				unsigned int vkc = static_cast<unsigned int>(wParam);
				HKL  layout = ::GetKeyboardLayout( 0 );
				unsigned int vk = MapVirtualKeyEx( vkc, 0, layout );

				mt::vec2f point;
				this->calcCursorPosition_( point );

				WChar tvk = translateVirtualKey_( vkc, vk );

				KeyCode code = (KeyCode)vkc;

				INPUT_SERVICE( m_serviceProvider )
					->pushKeyEvent( point.x, point.y, code, tvk, false, false );
			}break;
		case WM_SYSCOMMAND:
			{
				switch( wParam )
				{
				case SC_CLOSE:
					{
						m_close = true;
					}break;

				case SC_KEYMENU:
					{
						if( lParam == 13 )
						{
                            bool fullscreen = APPLICATION_SERVICE( m_serviceProvider )
                                ->getFullscreenMode();

                            APPLICATION_SERVICE( m_serviceProvider )
                                ->setFullscreenMode( !fullscreen );
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
				bool client = LOWORD( lParam ) == HTCLIENT;

				bool focus = APPLICATION_SERVICE( m_serviceProvider )
					->isFocus();

				if( focus == true && client == true && m_cursorMode == false )
				{
					::SetCursor( NULL );
				}
				else
				{
					if( m_cursor == NULL )
					{
						m_cursor = LoadCursor( NULL, IDC_ARROW );
					}

					::SetCursor( m_cursor );
				}

				return FALSE;
			}break;
		case WM_DESTROY:
			m_close = true;
				
			return 0;
		}

		LRESULT input_result;
		if( this->wndProcInput( hWnd, uMsg, wParam, lParam, input_result ) == true )
		{
			return input_result;
		}

		LRESULT result = WINDOWSLAYER_SERVICE(m_serviceProvider)
			->defWindowProc( hWnd, uMsg, wParam, lParam );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32Platform::wndProcInput( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT & _result )
	{
		(void)hWnd;

		bool handle = false;

		switch( uMsg )
		{
		case WM_TIMER:
			{
				if( wParam == UTIMER_MOUSE_EVENT )
				{
					m_mouseEvent.verify();
				}

				handle = true;
				_result = FALSE;
			}break;
		case UWM_MOUSE_LEAVE:
			{
				if( m_cursorInArea == true )
				{
					m_cursorInArea = false;
					InvalidateRect( hWnd, NULL, FALSE );
					UpdateWindow( hWnd );

					mt::vec2f point;
					this->calcCursorPosition_( point );

					INPUT_SERVICE( m_serviceProvider )
						->pushMouseLeaveEvent( 0, point.x, point.y, 0.f );

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
				this->calcCursorPosition_( point );

				m_mouseEvent.update();

				if( m_cursorInArea == false )
				{
					m_cursorInArea = true;

					InvalidateRect( hWnd, NULL, FALSE );
					UpdateWindow( hWnd );

					INPUT_SERVICE( m_serviceProvider )
						->pushMouseEnterEvent( 0, point.x, point.y, 0.f );
				}

				if( m_clickOutArea == true )
				{
					m_clickOutArea = false;

					if( (GetKeyState( VK_LBUTTON ) & 0x8000) == 0 )
					{
						INPUT_SERVICE( m_serviceProvider )
							->pushMouseButtonEvent( 0, point.x, point.y, 0, 0.f, false );
					}
				}

				int x = (int)(short)LOWORD( lParam );
				int y = (int)(short)HIWORD( lParam );

				if( m_lastMouse == false )
				{
					POINT p;
					GetCursorPos( &p );
					ScreenToClient( hWnd, &p );

					m_lastMouseX = p.x;
					m_lastMouseY = p.y;

					m_lastMouse = true;
				}

				int dx = x - m_lastMouseX;
				int dy = y - m_lastMouseY;

				if( dx == 0 && dy == 0 )
				{
					break;
				}

				m_lastMouseX = x;
				m_lastMouseY = y;

				float fdx = (float)dx;
				float fdy = (float)dy;

				RECT rect;
				if( GetClientRect( m_hWnd, &rect ) == FALSE )
				{
					return false;
				}

				float width = static_cast<float>(rect.right - rect.left);
				float height = static_cast<float>(rect.bottom - rect.top);

				fdx /= width;
				fdy /= height;

				//printf( "%f %f %d %d\n"
				//	, point.x
				//	, point.y
				//	, int( point.x * 1024.f )
				//	, int( point.y * 768.f )
				//	);

				INPUT_SERVICE( m_serviceProvider )
					->pushMouseMoveEvent( 0, point.x, point.y, fdx, fdy, 0.f );

				handle = true;
				_result = FALSE;
			}break;
		case WM_MOUSEWHEEL:
			{
				int zDelta = (int)(short)(HIWORD( wParam ));

				mt::vec2f point;
				this->calcCursorPosition_( point );

				int wheel = zDelta / WHEEL_DELTA;

				INPUT_SERVICE( m_serviceProvider )
					->pushMouseWheelEvent( point.x, point.y, 0, wheel );

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
				this->calcCursorPosition_( point );

				INPUT_SERVICE( m_serviceProvider )
					->pushMouseButtonEvent( 0, point.x, point.y, 0, 0.f, true );

				handle = true;
				_result = FALSE;
			}
			break;
		case WM_LBUTTONUP:
			{
				if( m_isDoubleClick == false )
				{
					mt::vec2f point;
					this->calcCursorPosition_( point );

					INPUT_SERVICE( m_serviceProvider )
						->pushMouseButtonEvent( 0, point.x, point.y, 0, 0.f, false );
				}

				m_isDoubleClick = false;

				handle = true;
				_result = FALSE;
			}break;
		case WM_RBUTTONDOWN:
			{
				//::SetFocus(m_hWnd);

				mt::vec2f point;
				this->calcCursorPosition_( point );

				INPUT_SERVICE( m_serviceProvider )
					->pushMouseButtonEvent( 0, point.x, point.y, 1, 0.f, true );

				handle = true;
				_result = FALSE;
			}break;
		case WM_RBUTTONUP:
			{
				if( m_isDoubleClick == false )
				{
					mt::vec2f point;
					this->calcCursorPosition_( point );

					INPUT_SERVICE( m_serviceProvider )
						->pushMouseButtonEvent( 0, point.x, point.y, 1, 0.f, false );
				}

				m_isDoubleClick = false;

				handle = true;
				_result = FALSE;
			}break;
		case WM_MBUTTONDOWN:
			{
				//::SetFocus(m_hWnd);

				mt::vec2f point;
				this->calcCursorPosition_( point );

				INPUT_SERVICE( m_serviceProvider )
					->pushMouseButtonEvent( 0, point.x, point.y, 2, 0.f, true );

				handle = true;
				_result = FALSE;
			}break;
		case WM_MBUTTONUP:
			{
				mt::vec2f point;
				this->calcCursorPosition_( point );

				INPUT_SERVICE( m_serviceProvider )
					->pushMouseButtonEvent( 0, point.x, point.y, 2, 0.f, false );

				handle = true;
				_result = FALSE;
			}break;
		case WM_KEYDOWN:
			{
				UINT vkc = static_cast<UINT>(wParam);
				HKL  layout = ::GetKeyboardLayout( 0 );
				UINT vk = MapVirtualKeyEx( vkc, 0, layout );

				mt::vec2f point;
				this->calcCursorPosition_( point );

				WChar tvk = this->translateVirtualKey_( vkc, vk );

				KeyCode code = (KeyCode)vkc;

				INPUT_SERVICE( m_serviceProvider )
					->pushKeyEvent( point.x, point.y, code, tvk, true, false );

				handle = true;
				_result = FALSE;
			}break;
		case WM_KEYUP:
			{
				UINT vkc = static_cast<UINT>(wParam);
				HKL  layout = ::GetKeyboardLayout( 0 );
				unsigned int vk = MapVirtualKeyEx( vkc, 0, layout );

				mt::vec2f point;
				this->calcCursorPosition_( point );

				WChar tvk = this->translateVirtualKey_( vkc, vk );

				KeyCode code = (KeyCode)vkc;

				INPUT_SERVICE( m_serviceProvider )
					->pushKeyEvent( point.x, point.y, code, tvk, false, false );

				handle = true;
				_result = FALSE;
			}break;
		}

		return handle;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32Platform::createWindow( const Resolution & _resolution, bool _fullscreen )
	{
		bool alreadyRunning = CONFIG_VALUE( m_serviceProvider, "Game", "AlreadyRunning", true );

		if( alreadyRunning == true )
		{
			m_alreadyRunningMonitor = new Win32AlreadyRunningMonitor( m_serviceProvider );

			if( m_alreadyRunningMonitor->run( EARP_SETFOCUS, MENGINE_WINDOW_CLASSNAME, m_projectTitle ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("Application invalid running monitor"
					);

				return false;
			}
		}

		m_windowResolution = _resolution;

		HBRUSH black_brush = (HBRUSH)GetStockObject( BLACK_BRUSH );

		// Register the window class		
		ATOM result = WINDOWSLAYER_SERVICE( m_serviceProvider )->registerClass(
			s_wndProc,
			0,
			0,
			m_hInstance,
			m_icon,
			black_brush,
			MENGINE_WINDOW_CLASSNAME
			);

		if( result == FALSE )
		{
			LOGGER_ERROR( m_serviceProvider )("Can't register window class");

			return false;
		}

		DWORD dwStyle = this->getWindowStyle_( _fullscreen );
		dwStyle &= ~WS_VISIBLE;

		RECT rc = this->getWindowsRect_( m_windowResolution, _fullscreen );

		DWORD exStyle = _fullscreen ? WS_EX_TOPMOST : 0;
		//DWORD exStyle = 0;

		m_hWnd = WINDOWSLAYER_SERVICE( m_serviceProvider )
            ->createWindowEx( exStyle, MENGINE_WINDOW_CLASSNAME, m_projectTitle.c_str(), dwStyle
			, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top
			, NULL, NULL, m_hInstance, (LPVOID)this );

		if( m_hWnd == NULL )
		{
			return false;
		}

		HWND hWndFgnd = ::GetForegroundWindow();

		if( hWndFgnd != m_hWnd )
		{
			LOGGER_WARNING( m_serviceProvider )("Setup Foreground Window...");

			::ShowWindow( m_hWnd, SW_MINIMIZE );
			::ShowWindow( m_hWnd, SW_RESTORE );

			//SetWindowPos(m_hWnd,hWndFgnd,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE);
			//SetWindowPos(hWndFgnd,m_hWnd,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
		}
		else
		{
			::ShowWindow( m_hWnd, SW_SHOW );
		}

		if( _fullscreen == true )
		{
			Resolution desktopResolution;
			this->getDesktopResolution( desktopResolution );

			this->notifyWindowModeChanged( desktopResolution, true );
		}

		::SetForegroundWindow( m_hWnd );          // Slightly Higher Priority
		::SetFocus( m_hWnd );                     // Sets Keyboard Focus To The Window
		::UpdateWindow( m_hWnd );

		m_mouseEvent.initialize( m_hWnd );

		bool vsync = APPLICATION_SERVICE( m_serviceProvider )
			->getVSync();

		bool maxfps = HAS_OPTION( m_serviceProvider, "maxfps" );

		if( maxfps == false && vsync == false )
		{
			m_fpsMonitor = new Win32FPSMonitor();
			m_fpsMonitor->initialize();

			m_fpsMonitor->setActive( true );
			m_fpsMonitor->setFrameTime( s_activeFrameTime );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
    Pointer Win32Platform::getWindowHandle() const
	{
		return m_hWnd;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32Platform::notifyWindowModeChanged( const Resolution & _resolution, bool _fullscreen )
	{
		LOGGER_WARNING( m_serviceProvider )("WinApplication::notifyWindowModeChanged %d:%d %d"
			, _resolution.getWidth()
			, _resolution.getHeight()
			, _fullscreen
			);

		m_windowResolution = _resolution;

		DWORD dwStyle = this->getWindowStyle_( _fullscreen );

		RECT rc = this->getWindowsRect_( m_windowResolution, _fullscreen );

		DWORD dwExStyle = WINDOWSLAYER_SERVICE(m_serviceProvider)
			->getWindowLong( m_hWnd, GWL_EXSTYLE );

		if( _fullscreen == false )
		{
			// When switching back to windowed mode, need to reset window size 
			// after device has been restored

			WINDOWSLAYER_SERVICE( m_serviceProvider )
				->setWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyle & (~WS_EX_TOPMOST) );

			WINDOWSLAYER_SERVICE( m_serviceProvider )
				->setWindowLong( m_hWnd, GWL_STYLE, dwStyle );

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
			WINDOWSLAYER_SERVICE( m_serviceProvider )
				->setWindowLong( m_hWnd, GWL_EXSTYLE, dwExStyle | WS_EX_TOPMOST );

			WINDOWSLAYER_SERVICE( m_serviceProvider )
				->setWindowLong( m_hWnd, GWL_STYLE, dwStyle );

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
	void Win32Platform::notifyVsyncChanged( bool _vsync )
	{
		m_vsync = _vsync;

		if( m_fpsMonitor != nullptr )
		{
			m_fpsMonitor->setActive( m_vsync == false );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32Platform::notifyCursorModeChanged( bool _mode )
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
	bool Win32Platform::notifyCursorIconSetup( const ConstString & _name, const FilePath & _path, const MemoryInterfacePtr & _buffer )
	{
		TMapCursors::iterator it_found = m_cursors.find( _path );

		if( it_found == m_cursors.end() )
		{
			if( _buffer->empty() == true )
			{
				LOGGER_ERROR( m_serviceProvider )("WinApplication::notifyCursorIconSetup %s buffer empty"
					, _path.c_str()
					);

				return false;
			}

            PathString icoFile;
            icoFile += "IconCache";
            icoFile += '_';
            icoFile += _path;
            icoFile += ".ico";
            
            FilePath c_icoFile = Helper::stringizeFilePath( m_serviceProvider, icoFile );

            OutputStreamInterfacePtr stream = FILE_SERVICE(m_serviceProvider)
				->openOutputFile( STRINGIZE_STRING_LOCAL( m_serviceProvider, "user" ), c_icoFile );

            if( stream == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("Win32Platform::notifyCursorIconSetup name %s path %s can't open output stream '%s'"
					, _name.c_str()
					, _path.c_str()
					, c_icoFile.c_str()
					);

				return false;
			}

			void * memory = _buffer->getMemory();
			size_t size = _buffer->getSize();

			if( stream->write( memory, size ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("Win32Platform::notifyCursorIconSetup name %s path %s can't write output stream '%s'"
					, _name.c_str()
					, _path.c_str()
					, c_icoFile.c_str()
					);

				return false;
			}

			stream->flush();
			stream = nullptr;

			WString unicode_icoFile;
			if( Helper::utf8ToUnicode( m_serviceProvider, c_icoFile, unicode_icoFile ) == false )
			{
				LOGGER_ERROR( m_serviceProvider )("Win32Platform::notifyCursorIconSetup name %s path %s can't file name '%s' to unicode"
					, _name.c_str()
					, _path.c_str()
					, c_icoFile.c_str()
					);

				return false;
			}

			HCURSOR cursor = ::LoadCursorFromFileW( unicode_icoFile.c_str() );

			if( cursor == NULL )
			{
				DWORD errCode = ::GetLastError();

				if( errCode != 0 )
				{
					LOGGER_ERROR( m_serviceProvider )("WinApplication::notifyCursorIconSetup %s for file %ls errCode %d"
						, _name.c_str()
						, unicode_icoFile.c_str()
						, errCode
						);

					return false;
				}
			}

			it_found = m_cursors.insert( std::make_pair( _name, cursor ) ).first;
		}

		m_cursor = it_found->second;

		::SetCursor( m_cursor );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	DWORD Win32Platform::getWindowStyle_( bool _fullsreen ) const
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

			//bool allowMaximize = m_application->getAllowFullscreenSwitchShortcut();

			//if( allowMaximize == true )
			//{
			//	dwStyle |= WS_MAXIMIZEBOX;
			//}
		}
		else
		{
			dwStyle |= WS_VISIBLE;
		}

		return dwStyle;
	}
	//////////////////////////////////////////////////////////////////////////
	RECT Win32Platform::getWindowsRect_( const Resolution & _resolution, bool _fullsreen ) const
	{
		RECT rc;
		::SetRect( &rc, 0, 0, (int)_resolution.getWidth(), (int)_resolution.getHeight() );

		if( _fullsreen == false )
		{
			DWORD dwStyle = this->getWindowStyle_( _fullsreen );
			::AdjustWindowRect( &rc, dwStyle, FALSE );

			RECT workArea;
			::SystemParametersInfo( SPI_GETWORKAREA, 0, &workArea, 0 );

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
	bool Win32Platform::calcCursorPosition_( mt::vec2f & _point ) const
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
	static void PathCorrectBackslash( WChar * _out, const WString & _in )
	{
		wcscpy( _out, _in.c_str() );

		WChar * pch = wcschr( _out, '/' );
		while( pch != NULL )
		{
			*pch = '\\';

			pch = wcschr( pch + 1, '/' );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32Platform::existDirectory( const WString & _path ) const
	{
		WChar pathCorrect[MENGINE_MAX_PATH];
		PathCorrectBackslash( pathCorrect, _path );

		size_t len = wcslen( pathCorrect );
		if( len == 0 )	// current dir
		{
			return true;
		}

		if( pathCorrect[len - 1] == L':' )	// root dir
		{
			return true;	// let it be
		}

		DWORD attributes = GetFileAttributes( pathCorrect );

		if( attributes == INVALID_FILE_ATTRIBUTES )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32Platform::createDirectory( const WString & _path )
	{
		WChar pathCorrect[MENGINE_MAX_PATH];
		PathCorrectBackslash( pathCorrect, _path );

		PathRemoveBackslash( pathCorrect );

		if( PathIsDirectoryW( pathCorrect ) == FILE_ATTRIBUTE_DIRECTORY )
		{
			return true;
		}

		TVectorWString paths;

		for( ;; )
		{
			paths.push_back( pathCorrect );

			if( PathRemoveFileSpecW( pathCorrect ) == FALSE )
			{
				break;
			}

			if( PathIsDirectoryW( pathCorrect ) == FILE_ATTRIBUTE_DIRECTORY )
			{
				break;
			}
		}

		for( TVectorWString::reverse_iterator
			it = paths.rbegin(),
			it_end = paths.rend();
			it != it_end;
			++it )
		{
			const WString & path = *it;

			BOOL successful = CreateDirectory( path.c_str(), NULL );

			if( successful == FALSE )
			{
				DWORD err = GetLastError();

				switch( err )
				{
				case ERROR_ALREADY_EXISTS:
					{
						LOGGER_WARNING( m_serviceProvider )("VistaWindowsLayer::createDirectory %ls alredy exists"
							, path.c_str()
							);

						return false;
					}break;
				case ERROR_PATH_NOT_FOUND:
					{
						LOGGER_WARNING( m_serviceProvider )("VistaWindowsLayer::createDirectory %ls not found"
							, path.c_str()
							);

						return false;
					}break;
				default:
					{
						LOGGER_WARNING( m_serviceProvider )("VistaWindowsLayer::createDirectory %ls unknown error %d"
							, path.c_str()
							, err
							);

						return false;
					}break;
				}
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32Platform::createDirectoryUserPicture( const WString & _path, const WString & _file, const void * _data, size_t _size )
	{
		bool successful = WINDOWSLAYER_SERVICE( m_serviceProvider )
			->createDirectoryUserPicture( _path, _file, _data, _size );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32Platform::createDirectoryUserMusic( const WString & _path, const WString & _file, const void * _data, size_t _size )
	{
		bool successful = WINDOWSLAYER_SERVICE( m_serviceProvider )
			->createDirectoryUserMusic( _path, _file, _data, _size );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32Platform::concatenateFilePath( const FilePath & _folder, const FilePath & _fileName, WChar * _filePath, size_t _capacity )
	{
		bool successful = WINDOWSLAYER_SERVICE( m_serviceProvider )
			->concatenateFilePath( _folder, _fileName, _filePath, _capacity );

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32Platform::getDesktopResolution( Resolution & _resolution ) const
	{
		int cxscreen = ::GetSystemMetrics( SM_CXSCREEN );
		int cyscreen = ::GetSystemMetrics( SM_CYSCREEN );

		_resolution.setWidth( cxscreen );
		_resolution.setHeight( cyscreen );

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Win32Platform::getCurrentPath( WChar * _path, size_t _len ) const
	{
		DWORD len = (DWORD)::GetCurrentDirectory( (DWORD)_len, _path );

		if( len == 0 )
		{
			return 0;
		}

		_path[len] = L'\\';
		_path[len + 1] = L'\0';

		return (size_t)len + 1;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Win32Platform::getUserPath( WChar * _path, size_t _len ) const
	{
		bool developmentMode = HAS_OPTION( m_serviceProvider, "dev" );
		bool roamingMode = HAS_OPTION( m_serviceProvider, "roaming" );
		bool noroamingMode = HAS_OPTION( m_serviceProvider, "noroaming" );

		if( developmentMode == true && roamingMode == false || noroamingMode == true )
		{
			WChar currentPath[MENGINE_MAX_PATH];

			DWORD len = (DWORD)::GetCurrentDirectory( MENGINE_MAX_PATH, currentPath );

			if( len == 0 )
			{
				LOGGER_ERROR( m_serviceProvider )("WinApplication::makeUserPath_: failed to get current directory"
					);

				return false;
			}

			currentPath[len] = L'\\';
			currentPath[len + 1] = L'\0';

			wcscat( currentPath, L"User" );
			wcscat( currentPath, L"\\" );

			wcscpy( _path, currentPath );

			size_t currentPathLen = wcslen( currentPath );

			return currentPathLen;
		}
		else	// create user directory in ~/Local Settings/Application Data/<uUserPath>/
		{
			WChar currentPath[MENGINE_MAX_PATH];
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

			BOOL result = SHGetPathFromIDListW( itemIDList, currentPath );

			if( result == FALSE )
			{
				LOGGER_ERROR( m_serviceProvider )("SHGetPathFromIDListW invalid"
					);

				return false;
			}

			CoTaskMemFree( itemIDList );

			wcschr( currentPath, L'\\' );
			WString wcompany = CONFIG_VALUE( m_serviceProvider, "Project", "Company", L"NONAME" );
			wcscat( currentPath, wcompany.c_str() );
			wcschr( currentPath, L'\\' );
			WString wname = CONFIG_VALUE( m_serviceProvider, "Project", "Name", L"UNKNOWN" );
			wcscat( currentPath, wname.c_str() );
			wcschr( currentPath, L'\\' );

			wcscpy( _path, currentPath );

			size_t currentPathLen = wcslen( currentPath );

			return currentPathLen;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32Platform::minimizeWindow()
	{
		::ShowWindow( m_hWnd, SW_MINIMIZE );
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32Platform::setCursorPosition( const mt::vec2f & _pos )
	{
		POINT cPos = {(int)_pos.x, (int)_pos.y};

		::ClientToScreen( m_hWnd, &cPos );

		::SetCursorPos( cPos.x, cPos.y );
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32Platform::showKeyboard()
	{
		//Empty - for iPad
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32Platform::hideKeyboard()
	{
		//Empty - for iPad
	}
	//////////////////////////////////////////////////////////////////////////
	WChar Win32Platform::translateVirtualKey_( unsigned int _vkc, unsigned int _vk ) const
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

		HKL layout = ::GetKeyboardLayout( 0 );

		WCHAR wide[4];
		int res = ::ToUnicodeEx( _vkc, _vk, keyState, wide, 4, 0, layout );

		if( res <= 0 )
		{
			return 0;
		}

		return wide[0];
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32Platform::onEvent( const ConstString & _event, const TMapParams & _params )
	{
		(void)_event;
		(void)_params;
		//Empty
	}
	//////////////////////////////////////////////////////////////////////////
	bool Win32Platform::openUrlInDefaultBrowser( const WString & _url )
	{
		//size_t cmd_path_size = 0;
		//wchar_t cmd_path [MENGINE_MAX_PATH];
		wchar_t * cmd_path = _wgetenv( L"COMSPEC" );

		WString params = L"/c start " + _url;

		STARTUPINFO startup_info;
		memset( &startup_info, 0, sizeof( startup_info ) );
		startup_info.cb = sizeof( startup_info );
		startup_info.wShowWindow = SW_HIDE;
		startup_info.dwFlags |= STARTF_USESHOWWINDOW;

		PROCESS_INFORMATION process_info;
		memset( &process_info, 0, sizeof( process_info ) );

		WCHAR lpCommandLine[32768] = {0};
		wcscpy( lpCommandLine, params.c_str() );

		BOOL result = ::CreateProcess(
			cmd_path,          // path
			lpCommandLine, // command line
			NULL,            // process attributes
			NULL,            // thread attributes
			FALSE,            // inherit handles
			NORMAL_PRIORITY_CLASS,    // creation flags
			NULL,            // environment
			NULL,            // current directory
			&startup_info,        // startup info structure
			&process_info        // process info structure
			);

		LOGGER_WARNING( m_serviceProvider )("WinApplication::openUrlInDefaultBrowser %ls %d"
			, _url.c_str()
			, result
			);

		if( result == FALSE )
		{
			return false;
		}

        CloseHandle( process_info.hProcess );
        CloseHandle( process_info.hThread );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Win32Platform::setActive_( bool _active )
	{
		if( m_active == _active )
		{
			return;
		}

		m_active = _active;

		if( m_fpsMonitor != nullptr )
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
		if( this->calcCursorPosition_( point ) == true )
		{
			INPUT_SERVICE( m_serviceProvider )
				->pushMousePositionEvent( 0, point.x, point.y, 0.f );
		}

		if( m_active == false )
		{
			INPUT_SERVICE( m_serviceProvider )
				->pushMouseLeaveEvent( 0, point.x, point.y, 0.f );
		}
		else
		{
			INPUT_SERVICE( m_serviceProvider )
				->pushMouseEnterEvent( 0, point.x, point.y, 0.f );
		}

		bool nopause = HAS_OPTION( m_serviceProvider, "nopause" );

		if( nopause == false )
		{
			APPLICATION_SERVICE( m_serviceProvider )
				->setFocus( m_active );

			INPUT_SERVICE( m_serviceProvider )
				->onFocus( m_active );

			bool turnSound = m_active;
			APPLICATION_SERVICE( m_serviceProvider )
				->turnSound( turnSound );
		}
		else
		{
			APPLICATION_SERVICE( m_serviceProvider )
				->setFocus( true );

			INPUT_SERVICE( m_serviceProvider )
				->onFocus( true );

			APPLICATION_SERVICE( m_serviceProvider )
				->turnSound( true );
		}

		if( m_active == true )
		{
			if( m_cursor == NULL )
			{
				m_cursor = LoadCursor( NULL, IDC_ARROW );
			}

			::SetCursor( m_cursor );
		}
		else
		{
			::SetCursor( NULL );
		}
	}
}	// namespace Menge
