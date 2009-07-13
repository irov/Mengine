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

#	include "resource.h"

#	include "Menge/Utils.h"

#	include <ctime>
#	include <algorithm>

#ifdef _MSC_VER
#	define snprintf _snprintf
#endif

const Menge::String config_file = "application.xml";

#	ifndef MENGE_MASTER_RELEASE
#		define LOG( message )\
	if( m_logSystem ) m_logSystem->logMessage( message + String("\n"), LM_LOG );
#	else
#		define LOG( message )
#	endif

#	define LOG_ERROR( message )\
	if( m_logSystem ) m_logSystem->logMessage( message + String("\n"), LM_ERROR );

namespace Menge
{
	static WCHAR s_logFileName[] = L"\\Menge.log";
	static WCHAR s_userPath[MAX_PATH] = L"";
	//////////////////////////////////////////////////////////////////////////
	static void s_logStackFrames( HANDLE _hFile, void* _faultAddress, char* eNextBP )
	{
		char wBuffer[4096];
		DWORD wr;
		char *p, *pBP;                                     
		unsigned i, x, BpPassed;
		static int  CurrentlyInTheStackDump = 0;
		//...
		BpPassed = (eNextBP != NULL);
		if(! eNextBP)
		{
#if defined _MSC_VER
			_asm mov     eNextBP, eBp   
#else
			asm("mov  %ebp, 12(%ebp);");
#endif
		}
		else 
		{
			sprintf( wBuffer, "\n  Fault Occurred At $ADDRESS:%08LX\n", _faultAddress );
			::WriteFile( _hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
		}
		// prevent infinite loops
		for(i = 0; eNextBP && i < 100; i++)
		{      
			pBP = eNextBP;           // keep current BasePointer
			eNextBP = *(char **)pBP; // dereference next BP 
			p = pBP + 8; 
			// Write 20 Bytes of potential arguments
			sprintf( wBuffer, "         with " );
			::WriteFile( _hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			for(x = 0; p < eNextBP && x < 20; p++, x++)
			{
				sprintf( wBuffer, "%02X ", *(unsigned char *)p );
				::WriteFile( _hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			}
			sprintf( wBuffer, "\n\n" );
			::WriteFile( _hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			if(i == 1 && ! BpPassed) 
			{
				sprintf( wBuffer, "Fault Occurred Here:\n" );
				::WriteFile( _hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			}
			// Write the backjump address
			sprintf( wBuffer, "*** %2d called from $ADDRESS:%08LX\n", i, *(char **)(pBP + 4) );
			::WriteFile( _hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			if(*(char **)(pBP + 4) == NULL)
			{
				break; 
			}
		}

		CurrentlyInTheStackDump = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	static LONG WINAPI s_exceptionHandler(EXCEPTION_POINTERS* pExceptionPointers)
	{
		EXCEPTION_RECORD* pRecord = pExceptionPointers->ExceptionRecord;
		CONTEXT* pContext = pExceptionPointers->ContextRecord;

		WCHAR fullFileName[MAX_PATH];
		wcsncpy( fullFileName, s_userPath, MAX_PATH );
		wcsncat( fullFileName, s_logFileName, MAX_PATH );

		HANDLE hFile = ::CreateFile( fullFileName, GENERIC_READ|GENERIC_WRITE, 
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
			snprintf( wBuffer, 4096, "Date: %02d.%02d.%d, %02d:%02d:%02d\n", tm.wDay, tm.wMonth, tm.wYear, tm.wHour, tm.wMinute, tm.wSecond );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ),&wr, 0 );
			snprintf( wBuffer, 4096, "OS: Windows %ld.%ld.%ld\n", os_ver.dwMajorVersion, os_ver.dwMinorVersion, os_ver.dwBuildNumber );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ),&wr, 0 );
			snprintf( wBuffer, 4096, "Source SVN Revision: %s", Menge::Application::getVersionInfo() );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ),&wr, 0 );
			strcpy( wBuffer, "\nCrash Info:\n" );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			snprintf( wBuffer, 4096, "Exception Code: 0x%08x\n", pRecord->ExceptionCode );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			snprintf( wBuffer, 4096, "Flags: 0x%08x\n", pRecord->ExceptionFlags );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			snprintf( wBuffer, 4096, "Address: 0x%08x\n\n", pRecord->ExceptionAddress );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			if( ( pContext->ContextFlags & CONTEXT_INTEGER ) != 0 )
			{
				snprintf( wBuffer, 4096, "Edi: 0x%08x\t Esi: 0x%08x\n", pContext->Edi, pContext->Esi );
				::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
				snprintf( wBuffer, 4096, "Ebx: 0x%08x\t Edx: 0x%08x\n", pContext->Ebx, pContext->Edx );
				::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
				snprintf( wBuffer, 4096, "Ecx: 0x%08x\t Eax: 0x%08x\n\n", pContext->Ecx, pContext->Eax );
				::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			}
			if( ( pContext->ContextFlags & CONTEXT_CONTROL ) != 0 )
			{
				snprintf( wBuffer, 4096, "Ebp: 0x%08x\t Eip: 0x%08x\n", pContext->Ebp, pContext->Eip );
				::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
				snprintf( wBuffer, 4096, "SegCs: 0x%08x\t EFlags: 0x%08x\n", pContext->SegCs, pContext->EFlags );
				::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
				snprintf( wBuffer, 4096, "Esp: 0x%08x\t SegSs: 0x%08x\n", pContext->Esp, pContext->SegSs );
				::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			}
			s_logStackFrames( hFile, pRecord->ExceptionAddress, (char*)pContext->Ebp );
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
	//////////////////////////////////////////////////////////////////////////
	static DWORD WINAPI s_threadFrameSignal(LPVOID lpParameter)
	{
		Menge::WinApplication* winApp = reinterpret_cast<Menge::WinApplication*>( lpParameter );
		return winApp->threadFrameSignal();
	}
	//////////////////////////////////////////////////////////////////////////
	static const unsigned long s_activeFrameTime = 16;
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
			int width = info.rcMonitor.right - info.rcMonitor.left;
			int height = info.rcMonitor.bottom - info.rcMonitor.top;
			app->setDesktopResolution( width, height );

			return FALSE;
		}
		return TRUE;
	}
	//////////////////////////////////////////////////////////////////////////
	WinApplication::WinApplication( HINSTANCE _hInstance, const String& _commandLine ) 
		: m_running( true )
		, m_frameTime( 0.f )
		, m_mutex( 0 )
		, m_focus( true )
		, m_name( "Mengine" )
		, m_hWnd(0)
		, m_hasWindowPanel( true )
		, m_cursorInArea( false )
		, m_fullscreen( false )
		, m_handleMouse( true )
		, m_hInstance( _hInstance )
		, m_loggerConsole( NULL )
		, m_commandLine( " " + _commandLine + " ")
		, m_menge( NULL )
		, m_hEvent( INVALID_HANDLE_VALUE )
		, m_hThread( INVALID_HANDLE_VALUE )
		, m_frameTiming( s_activeFrameTime )
		, m_lastMouseX( 0 )
		, m_lastMouseY( 0 )
		, m_vsync( false )
		, m_maxfps( false )
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
			delete m_menge;
			m_menge = NULL;
		}

		if( m_mutex )
		{
			::CloseHandle( m_mutex );
			m_mutex = 0;
		}

		if( m_winTimer != NULL )
		{
			delete static_cast<WinTimer*>(m_winTimer);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	TimerInterface * WinApplication::getTimer() const
	{
		return m_winTimer;
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
		bool enableDebug = false;
		bool docsAndSettings = false;

		::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX );
		::SetUnhandledExceptionFilter( &s_exceptionHandler );

		::timeBeginPeriod( 1 );

				
		String uUserPath;
		
		HRSRC hResouce = ::FindResource( NULL, MAKEINTRESOURCE( 101 ), RT_RCDATA );
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

		if( docsAndSettings == false )
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

		m_menge = new Application( this, uUserPath, scriptInit );
		m_menge->enableDebug( enableDebug );

		setlocale( LC_CTYPE, "" );

		m_logSystem = m_menge->initializeLogSystem();

		if( m_logSystem != NULL && m_commandLine.find( " -console " ) != String::npos )
		{
			m_loggerConsole = new LoggerConsole();
			m_logSystem->registerLogger( m_loggerConsole );

			LOG_ERROR( "LogSystem initialized successfully" );	// log message anyway
		}

		if( m_logSystem != NULL && m_commandLine.find( " -verbose " ) != String::npos )
		{
			m_logSystem->setVerboseLevel( LM_MAX );

			LOG( "Verbose logging mode enabled" );
		}

		m_menge->setLanguagePack( languagePack );

		/*wchar_t wProjName[MAX_PATH] = L"Menge"; 
		LoadString( m_hInstance, IDS_PROJECT_NAME, (LPWSTR)wProjName, MAX_PATH );

		HRESULT hr;
		wchar_t wUserPath[MAX_PATH] = L"";

		SHGetFolderPathAndSubDir( NULL,					//hWnd	
		CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,	//csidl
		NULL,										//hToken
		0,											//dwFlags
		wProjName,									//pszSubDir
		szPath);									//pszPath*/

		LOG( "Enumarating monitors..." );
		EnumDisplayMonitors( NULL, NULL, &s_monitorEnumProc, (LPARAM)this );

		m_menge->setDesktopResolution( Menge::Resolution( m_desktopWidth, m_desktopHeight ) );

		RECT workArea;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);

		RECT clientArea = workArea;
		::AdjustWindowRect( &clientArea, WS_OVERLAPPEDWINDOW, FALSE );
		size_t maxClientWidth = 2 * (workArea.right - workArea.left) - (clientArea.right - clientArea.left);
		size_t maxClientHeight = 2 * (workArea.bottom - workArea.top) - (clientArea.bottom - clientArea.top);
		m_menge->setMaxClientAreaSize( maxClientWidth, maxClientHeight );

		LOG( "Initializing Mengine..." );
		if( m_menge->initialize( config_file, m_commandLine, true ) == false )
		{
			return false;
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

		MEMORYSTATUS mem_st;
		GlobalMemoryStatus(&mem_st);
		std::sprintf( strbuffer, "Memory: %ldK total, %ldK free, %ldK Page file total, %ldK Page file free"
			, mem_st.dwTotalPhys/1024L
			, mem_st.dwAvailPhys/1024L
			, mem_st.dwTotalPageFile/1024L
			, mem_st.dwAvailPageFile/1024L );
		LOG( strbuffer );

		sprintf( strbuffer, "SVN Revision: %s", Menge::Application::getVersionInfo() );
		LOG( strbuffer );

		String title = m_menge->getProjectTitle();
		// try to create mutex to sure that we are not running already
		StringW titleW = StringConversion::utf8ToWChar( title );
		StringW mutexName = StringW( MENGE_TEXT("MengeMutex_") ) + titleW;
		m_mutex = ::CreateMutex( NULL, FALSE, mutexName.c_str() );
		DWORD error = ::GetLastError();
		// already running
		if( error == ERROR_ALREADY_EXISTS )
		{
			StringW message = StringW( MENGE_TEXT("Another instance of ") ) + titleW + StringW( MENGE_TEXT(" is already running") );
			::MessageBox( NULL, message.c_str(), titleW.c_str(), MB_ICONWARNING );
			return false;
		}


		LOG( "Creating Render Window..." );
		bool fullscreen = m_menge->getFullscreenMode();
		const Menge::Resolution& winRes = m_menge->getResolution();
		m_hasWindowPanel = m_menge->getHasWindowPanel();

		WindowHandle wh = createWindow( title, winRes[0], winRes[1], fullscreen, m_hasWindowPanel );

		if( m_menge->createRenderWindow( wh, wh ) == false )
		{
			return false;
		}

		LOG( "Initializing Game data..." );
		if( m_menge->initGame() == false )
		{
			return false;
		}

		m_vsync = m_menge->getVSync();

		if( m_vsync == false && m_maxfps == false )
		{
			m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			DWORD       threadId;
			m_hThread = CreateThread(NULL, 0, &s_threadFrameSignal, (LPVOID)this, 0, &threadId);
			SetThreadPriority(m_hThread, THREAD_PRIORITY_TIME_CRITICAL);
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	DWORD WinApplication::threadFrameSignal()
	{
		while( m_running )
		{
			Sleep( m_frameTiming );
			SetEvent( m_hEvent );
		}
		SetEvent( m_hEvent );
		return 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::loop()
	{
		MSG  msg;
		POINT pos;

		while( m_running )
		{
			while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			::GetCursorPos( &pos );
			//int dx = pos.x - m_lastMouseX;
			//int dy = pos.y - m_lastMouseY;
			//printf("MouseMove GetCursorPos %d %d\n", pos.x, pos.y );
			/*if( dx != 0 || dy != 0 )
			{
			if( !m_cursorInArea )
			{
			m_cursorInArea = true;
			::ShowCursor( FALSE );
			m_menge->onMouseEnter();
			}
			m_menge->injectMouseMove( dx, dy, 0 );
			}*/

			//m_lastMouseX = pos.x;
			//m_lastMouseY = pos.y;

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

			m_frameTime = m_winTimer->getDeltaTime();
			m_menge->onUpdate( m_frameTime );

			if( m_vsync == false && m_maxfps == false )
			{
				WaitForSingleObject(m_hEvent, INFINITE);
			}
		}

		// Clean up
		if( m_hWnd )
		{
			::DestroyWindow( m_hWnd );
			m_hWnd = NULL;
		}

		if( m_hEvent != INVALID_HANDLE_VALUE )
		{
			CloseHandle( m_hEvent );
		}

		if( m_hThread != INVALID_HANDLE_VALUE )
		{
			CloseHandle( m_hThread );
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
	WindowHandle WinApplication::createWindow( const Menge::String & _name, std::size_t _width, std::size_t _height, bool _fullscreen, bool _hasPanel )
	{
		DWORD dwStyle = WS_VISIBLE | WS_CLIPCHILDREN;
		RECT rc;

		m_windowWidth = _width;
		m_windowHeight = _height;

		if (!_fullscreen)
		{
			dwStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION ;//|	WS_SYSMENU | WS_MINIMIZEBOX;
			if( _hasPanel == true )
			{
				dwStyle |= WS_SYSMENU | WS_MINIMIZEBOX;
			}
		}
		else
		{
			dwStyle |= WS_POPUP;
		}

		// Calculate window dimensions required
		// to get the requested client area
		SetRect(&rc, 0, 0, (int)m_windowWidth, (int)m_windowHeight);
		AdjustWindowRect( &rc, dwStyle, FALSE );
		LONG width = rc.right - rc.left;
		LONG height = rc.bottom - rc.top;

		LONG left = 0;
		LONG top = 0;

		if( !_fullscreen )
		{
			RECT workArea;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
			left = (workArea.left + workArea.right - width) / 2;
			top = (workArea.top + workArea.bottom - height) / 2;
		}

		m_name = _name;
		//StringW nameW = Menge::Utils::AToW( m_name );
		StringW nameW = StringConversion::utf8ToWChar( m_name );

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

		//m_lastMouseX = left;
		//m_lastMouseY = top;
		// Create our main window
		// Pass pointer to self
		m_hWnd = ::CreateWindow( L"MengeWnd", nameW.c_str(), dwStyle,
			left, top, width, height, NULL, 0, m_hInstance, (LPVOID)this);

		ShowWindow( m_hWnd, SW_NORMAL );

		::GetWindowInfo( m_hWnd, &m_wndInfo);
		return static_cast<WindowHandle>( m_hWnd ); 
	}
	//////////////////////////////////////////////////////////////////////////
	LRESULT CALLBACK WinApplication::wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		POINT cPos;
		::GetCursorPos( &cPos );
		//printf( "some message %d GetCursorPos %d %d\n", uMsg, cPos.x, cPos.y );
		switch( uMsg )
		{
		/*case WM_ACTIVATE:
			m_focus = (LOWORD(wParam) != WA_INACTIVE);
			if( m_focus )
			{
				m_frameTiming = s_activeFrameTime;
			}
			else
			{
				m_frameTiming = s_inactiveFrameTime;
			}
			m_menge->onFocus( m_focus );
			break;*/
			/*case WM_SHOWWINDOW:
			case WM_ENABLE:
			m_focus = (wParam == TRUE);
			if( m_focus )
			{
			m_frameTiming = s_activeFrameTime;
			}
			else
			{
			m_frameTiming = s_inactiveFrameTime;
			}
			m_menge->onFocus( m_focus );
			break;*/
		case WM_KILLFOCUS:
			m_focus = false;
			m_frameTiming = s_inactiveFrameTime;
			m_menge->onFocus( m_focus );
			break;
		case WM_SETFOCUS:
			m_focus = true;
			m_frameTiming = s_activeFrameTime;
			m_menge->onFocus( m_focus );
			break;
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
			return TRUE;
			break;
		case WM_MOUSEMOVE:
			//if( m_handleMouse )
			{
				if( !m_cursorInArea )
				{
					m_cursorInArea = true;
					::ShowCursor( FALSE );
					m_menge->onMouseEnter();
				}
				int x = (int)(short)LOWORD(lParam);
				int y = (int)(short)HIWORD(lParam);
				int dx = x - m_lastMouseX;
				int dy = y - m_lastMouseY;
				m_menge->injectMouseMove( dx, dy, 0 );
				POINT cPos;
				::GetCursorPos( &cPos );
				::ScreenToClient( m_hWnd, &cPos );
				m_menge->setMousePosition( cPos.x, cPos.y );
				//printf("%d %d %d %d %d %d\n", cPos.x, cPos.y, m_lastMouseX, m_lastMouseY, cPos.x - m_lastMouseX, cPos.y - m_lastMouseY );
				m_lastMouseX = x;
				m_lastMouseY = y;
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
			dwStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION;// |	WS_SYSMENU | WS_MINIMIZEBOX;
			if( m_hasWindowPanel )
			{
				dwStyle |= WS_SYSMENU | WS_MINIMIZEBOX;
			}

			// When switching back to windowed mode, need to reset window size 
			// after device has been restored
			RECT rc;
			SetRect(&rc, 0, 0, (LONG)_width, (LONG)_height);
			//AdjustWindowRect(&rc, GetWindowLong(m_hWnd, GWL_STYLE), false);
			AdjustWindowRect(&rc, dwStyle, false);
			int winWidth = rc.right - rc.left;
			int winHeight = rc.bottom - rc.top;
			RECT workArea;
			SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
			int left = (workArea.left + workArea.right - winWidth) / 2;
			int top = (workArea.top + workArea.bottom - winHeight) / 2;

			SetWindowLong( m_hWnd, GWL_STYLE, dwStyle );
			SetWindowPos(m_hWnd, HWND_NOTOPMOST, left, top, winWidth, winHeight,
				SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOACTIVATE);
		}
		else
		{
			dwStyle |= WS_POPUP;
			SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
			SetWindowPos( m_hWnd, HWND_TOPMOST, 0, 0, (LONG)_width, (LONG)_height, SWP_NOZORDER | SWP_NOACTIVATE  | SWP_FRAMECHANGED);
		}
		::ShowWindow( m_hWnd, SW_NORMAL );
		//::SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
		::GetWindowInfo( m_hWnd, &m_wndInfo);
		//m_listener->onWindowMovedOrResized();
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::setHandleMouse( bool _handle )
	{
		m_handleMouse = _handle;
		/*if( _handle == true )
		{
		POINT pos;
		::GetCursorPos( &pos );
		//ScreenToClient( m_hWnd, &pos );
		m_menge->setMousePosition( pos.x - m_wndInfo.rcClient.left, pos.y - m_wndInfo.rcClient.top );
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::setCursorPosition( int _x, int _y )
	{
		/*	int screenw = GetSystemMetrics(SM_CXSCREEN);
		int screenh = GetSystemMetrics(SM_CYSCREEN);
		int left = (screenw - 1024) / 2;
		int top = (screenh - 768) / 2;*/
		//printf( "SetCursorPos %d %d\n", _x, _y );
		m_lastMouseX = _x;
		m_lastMouseY = _y;
		POINT cPos = { _x, _y };
		ClientToScreen( m_hWnd, &cPos );
		//printf( "setCursorPosition( %d, %d ), m_lastMouseX = %d, m_lastMouseY = %d\n", _x, _y, m_lastMouseX, m_lastMouseY );
		//BOOL res = ::SetCursorPos( cPos.x, cPos.y );
		//printf("%d\n", res );
		//POINT cPos;
		::GetCursorPos( &cPos );
		//printf("GetCursorPos %d %d\n", cPos.x, cPos.y );

		//POINT cPos;
		//::GetCursorPos( &cPos );
		//printf( "cursorPos %d %d\n", cPos.x, cPos.y );
	}
	//////////////////////////////////////////////////////////////////////////
	void WinApplication::showMessageBox( const String& _message, const String& _header, unsigned int _style )
	{
		//StringW message_w = Utils::AToW( _message );
		//StringW header_w = Utils::AToW( _header );
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
}	// namespace Menge

