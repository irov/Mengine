#	include "OgreApplication.h"

#	include "Menge/Application.h"

#	include "System/OgreInputSystem/OgreInputSystemInterface.h"
#	include "System/OgreRenderSystem/OgreRenderSystemInterface.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/SoundSystemInterface.h"
#	include "Interface/ParticleSystemInterface.h"
#	include "Interface/FileSystemInterface.h"


#	include "OIS/OIS.h"

#	include "SystemDLL.h"

#	include "XmlParser/XmlParser.h"

#	include <map>

//////////////////////////////////////////////////////////////////////////
bool initInterfaceSystem( ApplicationInterface** _ptrInterface )
{
	try
	{
		*_ptrInterface = new OgreApplication();
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
	delete static_cast<OgreApplication*>(_ptrInterface);
}
//////////////////////////////////////////////////////////////////////////
Ogre::RenderWindow* OgreApplication::m_renderWindow = NULL;
Menge::Application* OgreApplication::m_application = NULL;
HWND OgreApplication::m_hWnd = 0;
bool OgreApplication::m_cursorInArea = false;
WINDOWINFO OgreApplication::m_wndInfo;

OgreApplication::OgreApplication() 
: m_root(0)
//, m_hWnd(0)
, m_running( true )
, m_frameTime( 0.0f )
//, m_cursorInArea( false )
{}
//////////////////////////////////////////////////////////////////////////
OgreApplication::~OgreApplication()
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
}
//////////////////////////////////////////////////////////////////////////
bool OgreApplication::init( const char * _xmlFile )
{
	m_application = new Menge::Application();

#ifdef _DEBUG
	std::string DllModuleSetting = "DllModuleDebug";
	std::string DllLogSystem = "Systems_d/OgreLogSystem_d.dll";
	std::string DllFileSystem = "Systems_d/OgreFileSystem_d.dll";
	std::string DllRenderSystem = "Systems_d/OgreRenderSystem_d.dll";
	std::string DllInputSystem = "Systems_d/OgreInputSystem_d.dll";
	std::string DllSoundSystem = "Systems_d/SoundSystem_d.dll";
	std::string DllParticleSystem = "Systems_d/AstralaxParticleSystem_d.dll";
#else
	std::string DllModuleSetting = "DllModuleRelease";
	std::string DllLogSystem = "Systems/OgreLogSystem.dll";
	std::string DllFileSystem = "Systems/OgreFileSystem.dll";
	std::string DllRenderSystem = "Systems/OgreRenderSystem.dll";
	std::string DllInputSystem = "Systems/OgreInputSystem.dll";
	std::string DllSoundSystem = "Systems/SoundSystem.dll";
	std::string DllParticleSystem = "Systems/AstralaxParticleSystem.dll";
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

	m_resourcePath = fileInterface->platformBundlePath();

	m_root = new Ogre::Root(
		"",
		"",
		m_resourcePath + "Menge.log");

	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	fileInterface->loadPath(".");

	bool initialize = m_application->initialize( _xmlFile );

	initParams();

	m_renderWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;

	OIS::ParamList pl;

	pl.insert( std::make_pair(	"WINDOW", windowHndStr.str() ) );


	inputInterface->init( pl );
	m_application->createGame();

	renderInterface->init( m_root, m_renderWindow );

	return initialize;
}
//////////////////////////////////////////////////////////////////////////
void OgreApplication::initParams()
{
	int bits = m_application->getScreenBits();
	int screenWidth = m_application->getScreenWidth();
	int screenHeight = m_application->getScreenHeight();
	bool fullscreen = m_application->isFullScreen();
	std::string renderDriver = m_application->getRenderDriver();

	#ifdef _DEBUG
		renderDriver += "_d.dll";
	#else
		renderDriver += ".dll";
	#endif

	m_root->loadPlugin( m_resourcePath + renderDriver );

	Ogre::RenderSystem * renderSystem = m_root->getAvailableRenderers()->at( 0 );
	m_root->setRenderSystem( renderSystem );

	m_root->initialise( false );

	m_root->addFrameListener( this );

	createWindow( screenWidth, screenHeight, fullscreen );

	Ogre::NameValuePairList params;
	params.insert( std::make_pair("Colour Depth", Ogre::StringConverter::toString( bits ) ) );
	params.insert( std::make_pair( "externalWindowHandle", Ogre::StringConverter::toString( ( (unsigned int)m_hWnd)  ) ) );

	m_renderWindow = m_root->createRenderWindow( "Menge-engine", screenWidth, screenHeight, fullscreen, &params );
}
//////////////////////////////////////////////////////////////////////////
bool OgreApplication::frameStarted( const Ogre::FrameEvent &evt)
{
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool OgreApplication::frameEnded( const Ogre::FrameEvent &evt)
{
	const Ogre::RenderTarget::FrameStats& stats = m_renderWindow->getStatistics();
	m_frameTime = evt.timeSinceLastFrame;
	//printf("fps = %f \n", stats.avgFPS);
	//m_application->frameEnded();
	return true;
}
//////////////////////////////////////////////////////////////////////////
void OgreApplication::run()
{
	MSG  msg;
	POINT pos;
	while( m_running )
	{
		/*::GetCursorPos( &pos );
		if( m_cursorInArea 
			&& ( pos.x < m_wndInfo.rcClient.left 
			|| pos.x > m_wndInfo.rcClient.right 
			|| pos.y < m_wndInfo.rcClient.top
			|| pos.y > m_wndInfo.rcClient.bottom ) )
		{
			::ShowCursor( TRUE );
			m_application->handleMouseLeave();
		}*/
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		m_running = m_renderWindow->isClosed() == false;
		if( m_renderWindow->isActive() )
		{
			//Ogre::WindowEventUtilities::messagePump();
			m_running = m_application->update(  m_frameTime * 1000.f ) && m_renderWindow->isClosed() == false;
			m_root->renderOneFrame();
		}

		::Sleep(1);
	}
}
//////////////////////////////////////////////////////////////////////////
void OgreApplication::createWindow( unsigned int _width, unsigned int _height, bool _fullscreen )
{
	DWORD dwStyle = WS_VISIBLE | WS_CLIPCHILDREN;

	int width = _width;
	int height = _height;

	int screenw = ::GetSystemMetrics(SM_CXSCREEN);
	int screenh = ::GetSystemMetrics(SM_CYSCREEN);
	if ( width > screenw )
		width = screenw;
	if ( height > screenh )
		height = screenh;
	int left = (screenw - width) / 2;
	int top = (screenh - height) / 2;


	if (!_fullscreen)
	{
		dwStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION |	WS_SYSMENU | WS_MINIMIZEBOX;
	}
	else
	{
		dwStyle |= WS_POPUP;
		top = left = 0;
	}

	HINSTANCE hInstance = ::GetModuleHandle( NULL );
	// Register the window class
	// NB allow 4 bytes of window data for D3D9RenderWindow pointer
	WNDCLASS wc = 
	{ 0,
	OgreApplication::_wndProc,
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
LRESULT CALLBACK OgreApplication::_wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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
	/*case WM_MOUSEMOVE:
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
		break;*/
	}
	return ::DefWindowProc( hWnd, uMsg, wParam, lParam );
}
