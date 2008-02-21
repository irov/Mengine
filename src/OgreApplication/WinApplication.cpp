#	include "WinApplication.h"

#	include "Menge/Application.h"

#	include "System/OgreInputSystem/OgreInputSystemInterface.h"
#	include "System/OgreRenderSystem/OgreRenderSystemInterface.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/SoundSystemInterface.h"
#	include "Interface/ParticleSystemInterface.h"
#	include "Interface/FileSystemInterface.h"


#	include "OIS/OIS.h"

#	include "SystemDLL.h"

#	include <map>

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
Ogre::RenderWindow* WinApplication::m_renderWindow = NULL;
Menge::Application* WinApplication::m_application = NULL;
HWND WinApplication::m_hWnd = 0;
bool WinApplication::m_cursorInArea = false;
WINDOWINFO WinApplication::m_wndInfo;

WinApplication::WinApplication() 
: m_root(0)
//, m_hWnd(0)
, m_running( true )
, m_frameTime( 0.0f )
//, m_cursorInArea( false )
, m_mutex(0)
, m_focus( true )
{}
//////////////////////////////////////////////////////////////////////////
WinApplication::~WinApplication()
{
	m_application->finalize();

	delete m_application;

	for( TListApplicationDLL::iterator
		it = m_listApplicationDLL.begin(),
		it_end = m_listApplicationDLL.end();
	it != it_end;
	++it)
	{
		delete *it;
	}

	if( m_renderWindow )
	{
		m_renderWindow->destroy();
		m_renderWindow = 0;
	}

	if( m_root ) 
	{			
		Ogre::RenderWindow * window = m_root->getAutoCreatedWindow();

		if( window != NULL )
		{
			window->removeAllViewports();
		}

		delete m_root;
		m_root = 0;
	}

	if( m_mutex )
	{
		::CloseHandle( m_mutex );
		m_mutex = 0;
	}
}
//////////////////////////////////////////////////////////////////////////
bool WinApplication::init( const char * _xmlFile, const char * _args )
{

	std::string args;

	if( _args != NULL )
	{
		args = _args;
	}

	m_application = new Menge::Application( args );

#ifdef _DEBUG
	std::string DllModuleSetting = "DllModuleDebug";
	std::string DllLogSystem = "Systems/OgreLogSystem_d.dll";
	std::string DllFileSystem = "Systems/OgreFileSystem_d.dll";
	std::string DllRenderSystem = "Systems/OgreRenderSystem_d.dll";
	std::string DllInputSystem = "Systems/OgreInputSystem_d.dll";
	std::string DllSoundSystem = "Systems/ALSoundSystem_d.dll";
	std::string DllParticleSystem = "Systems/AstralaxParticleSystem_d.dll";
	std::string DllPhysicSystem = "Systems/PhysXPhysicSystem_d.dll";
#else
	std::string DllModuleSetting = "DllModuleRelease";
	std::string DllLogSystem = "Systems/OgreLogSystem.dll";
	std::string DllFileSystem = "Systems/OgreFileSystem.dll";
	std::string DllRenderSystem = "Systems/OgreRenderSystem.dll";
	std::string DllInputSystem = "Systems/OgreInputSystem.dll";
	std::string DllSoundSystem = "Systems/ALSoundSystem.dll";
	std::string DllParticleSystem = "Systems/AstralaxParticleSystem.dll";
	std::string DllPhysicSystem = "Systems/PhysXPhysicSystem.dll";
#endif

	printf("use log system [%s]\n", DllLogSystem.c_str() );

	LogSystemInterface * logInterface
		= addSystem<LogSystemInterface>( DllLogSystem );

	m_application->setLogSystem( logInterface );

	printf("use file system [%s]\n", DllFileSystem.c_str() );

	FileSystemInterface * fileInterface
		= addSystem<FileSystemInterface>( DllFileSystem );

	m_application->setFileSystem( fileInterface );

	printf("use input system [%s]\n", DllInputSystem.c_str() );

	OgreInputSystemInterface * inputInterface
		= addSystem<OgreInputSystemInterface>( DllInputSystem );

	m_application->setInputSystem( inputInterface );

	printf("use render system [%s]\n", DllRenderSystem.c_str() );

	OgreRenderSystemInterface * renderInterface
		= addSystem<OgreRenderSystemInterface>( DllRenderSystem );

	m_application->setRenderSystem( renderInterface );

	printf("use sound system [%s]\n", DllSoundSystem.c_str() );

	SoundSystemInterface * soundInterface
		= addSystem<SoundSystemInterface>( DllSoundSystem );

	m_application->setSoundSystem( soundInterface );

	printf("use particle system [%s]\n", DllParticleSystem.c_str() );

	ParticleSystemInterface * particleInterface
		= addSystem<ParticleSystemInterface>( DllParticleSystem );

	m_application->setParticleSystem( particleInterface );

	printf("use physic system [%s]\n", DllPhysicSystem.c_str() );

	PhysicSystemInterface * physicInterface
		= addSystem<PhysicSystemInterface>( DllPhysicSystem );

	m_application->setPhysicSystem( physicInterface );


	m_resourcePath = fileInterface->platformBundlePath();

	m_root = new Ogre::Root(
		"",
		"",
		m_resourcePath + "Menge.log");

	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	//Ogre::LogManager::getSingleton().setLogDetail( Ogre::LL_LOW );

	fileInterface->loadPath(".");

	bool initialize = m_application->initialize( _xmlFile );

	const std::string& title = m_application->getTitle();

	/// patch for ansi names
	char *ansistr = NULL;
	int length = MultiByteToWideChar(CP_UTF8, 0, m_application->getTitle().c_str(), m_application->getTitle().length(), NULL, NULL );
	WCHAR *lpszW = NULL;

	lpszW = new WCHAR[length+1];
	ansistr = ( char * ) calloc ( sizeof(char), length+5 );

	//this step intended only to use WideCharToMultiByte
	MultiByteToWideChar(CP_UTF8, 0, m_application->getTitle().c_str(), -1, lpszW, length );

	//Conversion to ANSI (CP_ACP)
	WideCharToMultiByte(CP_ACP, 0, lpszW, -1, ansistr, length, NULL, NULL);

	ansistr[length] = 0;

	delete[] lpszW;
	////

	m_mutex = ::CreateMutexA( NULL, FALSE, title.c_str() );
	DWORD error = ::GetLastError();

	if( error == ERROR_ALREADY_EXISTS )
	{
		std::string message = std::string("Another instance of ") + std::string( ansistr ) + std::string(" is already running");
		::MessageBoxA( NULL, message.c_str(), title.c_str(), MB_ICONWARNING );
		return false;
	}

	initParams();

	m_renderWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;

	OIS::ParamList pl;

	pl.insert( std::make_pair(	"WINDOW", windowHndStr.str() ) );


	inputInterface->init( pl );
	renderInterface->init( m_root, m_renderWindow );

	//fileInterface->loadPath( m_application->getResourcePath().c_str() );
	// We cant't use loadPath 'cause of not all libs uses fileSystem interface
	fileInterface->changeDir( m_application->getResourcePath().c_str() );
	//Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	m_application->createGame();

	return initialize;
}
//////////////////////////////////////////////////////////////////////////
void WinApplication::initParams()
{
	int bits = m_application->getScreenBits();
	int screenWidth = m_application->getScreenWidth();
	int screenHeight = m_application->getScreenHeight();
	bool fullscreen = m_application->isFullScreen();
	std::string renderDriver = m_application->getRenderDriver();
	bool vsync = m_application->getVSync();

	#ifdef _DEBUG
		renderDriver += "_d.dll";
//		m_root->loadPlugin( "Plugin_TheoraVideoSystem_d.dll" );
	#else
		renderDriver += ".dll";
		m_root->loadPlugin( "Plugin_TheoraVideoSystem.dll" );
	#endif

	m_root->loadPlugin( m_resourcePath + renderDriver );

	//m_root->loadPlugin( m_resourcePath + "Plugins/Plugin_BSPSceneManager_d.dll" );


	Ogre::RenderSystem * renderSystem = m_root->getAvailableRenderers()->at( 0 );
	m_root->setRenderSystem( renderSystem );

	m_root->initialise( false );

	m_root->addFrameListener( this );

	createWindow( screenWidth, screenHeight, fullscreen );

	Ogre::NameValuePairList params;
	params.insert( std::make_pair("Colour Depth", Ogre::StringConverter::toString( bits ) ) );
	params.insert( std::make_pair("vsync", Ogre::StringConverter::toString( vsync ) ) );
	params.insert( std::make_pair( "externalWindowHandle", Ogre::StringConverter::toString( ( (unsigned int)m_hWnd)  ) ) );
	
	/// patch for ansi names
	char *ansistr = NULL;
	int length = MultiByteToWideChar(CP_UTF8, 0, m_application->getTitle().c_str(), m_application->getTitle().length(), NULL, NULL );
	WCHAR *lpszW = NULL;

	lpszW = new WCHAR[length+1];
	ansistr = ( char * ) calloc ( sizeof(char), length+5 );

	//this step intended only to use WideCharToMultiByte
	MultiByteToWideChar(CP_UTF8, 0, m_application->getTitle().c_str(), -1, lpszW, length );

	//Conversion to ANSI (CP_ACP)
	WideCharToMultiByte(CP_ACP, 0, lpszW, -1, ansistr, length, NULL, NULL);

	ansistr[length] = 0;

	delete[] lpszW;
	////

	m_renderWindow = m_root->createRenderWindow( ansistr, screenWidth, screenHeight, fullscreen, &params );
}
//////////////////////////////////////////////////////////////////////////
bool WinApplication::frameStarted( const Ogre::FrameEvent &evt)
{
	//m_application->frameStarted();
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool WinApplication::frameEnded( const Ogre::FrameEvent &evt)
{
	const Ogre::RenderTarget::FrameStats& stats = m_renderWindow->getStatistics();
	m_frameTime = evt.timeSinceLastFrame;
	//printf("fps = %f \n", stats.avgFPS);
	return true;
}
//////////////////////////////////////////////////////////////////////////
void WinApplication::run()
{
	MSG  msg;
	static bool resetTime = false;
	POINT pos;
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
			m_application->handleMouseLeave();
			m_cursorInArea = false;
		}
		while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		m_running = m_renderWindow->isClosed() == false;
		
		if( m_renderWindow->isActive() )
		{
			if( !m_focus )
			{
				m_focus = true;
				m_application->onFocus( true );
				resetTime = true;
			}
			Ogre::WindowEventUtilities::messagePump();
			m_running = m_application->update(  m_frameTime * 1000.f ) && m_renderWindow->isClosed() == false;
			m_root->renderOneFrame();

			if ( resetTime )
			{
				m_frameTime = 0.0f;
				resetTime = false;
			}
		}
		else if( !m_renderWindow->isActive() && m_focus )
		{
			m_focus = false;
			m_application->onFocus( false );
		}

		::Sleep(1);
	}
}
//////////////////////////////////////////////////////////////////////////
void WinApplication::createWindow( unsigned int _width, unsigned int _height, bool _fullscreen )
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



	HINSTANCE hInstance = ::GetModuleHandle( NULL );
	// Register the window class
	// NB allow 4 bytes of window data for D3D9RenderWindow pointer
	WNDCLASS wc = 
	{ 0,
	WinApplication::_wndProc,
	0, 
	0, 
	hInstance,
	LoadIcon(0, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
	(HBRUSH)GetStockObject(BLACK_BRUSH), 0, L"MengeWnd" 
	};
	::RegisterClass(&wc);

	// Create our main window
	// Pass pointer to self
	m_hWnd = ::CreateWindow(L"MengeWnd", L"Menge-engine", dwStyle,
		left, top, width, height, NULL, 0, hInstance, this);

	TRACKMOUSEEVENT mouseEvent;
	mouseEvent.cbSize = sizeof( TRACKMOUSEEVENT );
	mouseEvent.dwFlags = TME_LEAVE;
	mouseEvent.hwndTrack = m_hWnd;
	::TrackMouseEvent( &mouseEvent );
 
	::GetWindowInfo( m_hWnd, &m_wndInfo);
}
//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WinApplication::_wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
	case WM_ACTIVATE:
		{
			::GetWindowInfo( m_hWnd, &m_wndInfo);
			bool active = (LOWORD(wParam) != WA_INACTIVE);
			if(m_renderWindow)
				m_renderWindow->setActive( active );
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
		if( m_renderWindow )
			m_renderWindow->windowMovedOrResized();
		break;
	case WM_DISPLAYCHANGE:
		::GetWindowInfo( m_hWnd, &m_wndInfo);
		if( m_renderWindow )
			m_renderWindow->windowMovedOrResized();
		break;
	case WM_SIZE:
		::GetWindowInfo( m_hWnd, &m_wndInfo);
		if(m_renderWindow)
			m_renderWindow->windowMovedOrResized();
		break;
	case WM_GETMINMAXINFO:
		// Prevent the window from going smaller than some minimu size
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
		break;
	case WM_CLOSE:

		m_renderWindow->destroy();
		break;
	case WM_MOUSEMOVE:
		if( !m_cursorInArea )
		{
			m_cursorInArea = true;
			::ShowCursor( FALSE );
			m_application->handleMouseEnter();
		}
		m_application->handleMouseMove( (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), 0 );
		break;
	case WM_LBUTTONDOWN:
		m_application->handleMouseButtonEvent( 1, true );
		break;
	case WM_LBUTTONUP:
		m_application->handleMouseButtonEvent( 1, false );
		break;
	case WM_RBUTTONDOWN:
		m_application->handleMouseButtonEvent( 2, true );
		break;
	case WM_RBUTTONUP:
		m_application->handleMouseButtonEvent( 2, false );
		break;
	case WM_MBUTTONDOWN:
		m_application->handleMouseButtonEvent( 3, true );
		break;
	case WM_MBUTTONUP:
		m_application->handleMouseButtonEvent( 3, false );
		break;
	}
	return ::DefWindowProc( hWnd, uMsg, wParam, lParam );
}
