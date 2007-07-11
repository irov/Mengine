#	include "WinApplication.h"

#	include "SystemDLL.h"

#	include "Menge/Application.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/WinInputSystemInterface.h"
#	include "Interface/WinRenderSystemInterface.h"

#	include "XmlParser/XmlParser.h"

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
: m_application(0)
{

}
//////////////////////////////////////////////////////////////////////////
WinApplication::~WinApplication()
{
	for each( Menge::SystemDLL * sysDll in m_listApplicationDLL )
	{
		delete sysDll;
	}

	delete m_application;
}
//////////////////////////////////////////////////////////////////////////
static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
//////////////////////////////////////////////////////////////////////////
bool WinApplication::init( const char * _xmlFile )
{
	m_application = new Menge::Application;
	//m_application->init( _xmlFile );

	std::string DllModuleSetting = "DllModule";
#ifdef _DEBUG
	DllModuleSetting += "Debug";
#else
	DllModuleSetting += "Release";
#endif

	typedef std::map<int, std::list<std::string>> TPackHierarchical;
	TPackHierarchical packHierarchical;

	int m_width;
	int m_height;
	int m_bits;

	bool m_fullScreen;

	std::string gameFile;

	FileSystemInterface * fileInterface = 0;
	WinInputSystemInterface * inputInterface = 0;
	WinRenderSystemInterface * renderInterface = 0;

	printf("parse application xml [%s] ...\n", _xmlFile );
	XML_PARSE_FILE( _xmlFile )
	{
		XML_CHECK_NODE_FOR_EACH("Application")
		{
			XML_CHECK_NODE_FOR_EACH( DllModuleSetting )
			{
				XML_DEF_ATTRIBUTES_NODE(DllFile);

				if( DllFile.empty() )
				{
					XML_CONTINUE();
				}

				XML_CHECK_NODE("FileSystem")
				{
					printf("use file system [%s]\n", DllFile.c_str() );

					Menge::SystemInterfaceDLL<FileSystemInterface> * interfaceDLL = 
						new Menge::SystemInterfaceDLL<FileSystemInterface>(DllFile);

					m_listApplicationDLL.push_back( interfaceDLL );

					fileInterface = interfaceDLL->getInterface();
					m_application->setFileSystem( fileInterface );
				}

				XML_CHECK_NODE("InputSystem")
				{
					printf("use input system [%s]\n", DllFile.c_str() );

					Menge::SystemInterfaceDLL<WinInputSystemInterface> * interfaceDLL = 
						new Menge::SystemInterfaceDLL<WinInputSystemInterface>(DllFile);

					m_listApplicationDLL.push_back( interfaceDLL );

					inputInterface = interfaceDLL->getInterface();
					m_application->setInputSystem( inputInterface );
				}

				XML_CHECK_NODE("RenderSystem")
				{
					printf("use render system [%s]\n", DllFile.c_str() );

					Menge::SystemInterfaceDLL<WinRenderSystemInterface> * interfaceDLL = 
						new Menge::SystemInterfaceDLL<WinRenderSystemInterface>(DllFile);

					m_listApplicationDLL.push_back( interfaceDLL );

					renderInterface = interfaceDLL->getInterface();
					m_application->setRenderSystem( renderInterface );
				}

				XML_CHECK_NODE("SoundSystem")
				{
					printf("use sound system [%s]\n", DllFile.c_str() );
					//Holder<SoundEngine>::keep( new SoundEngine(DllFile) ); 
				}

				XML_CHECK_NODE("Codec")
				{
					//	Decoder::DecoderComponent * dc = new Decoder::DecoderComponent(DllFile);
					//dc->
					//	Decoder::regDecoder("mng",dc);
					//m_soundEngine = new SoundEngine(DllFile); 
				}
			}

			XML_CHECK_NODE_FOR_EACH("Paks")
			{
				XML_CHECK_NODE("Pak")
				{
					std::string file;
					int priority;

					XML_VALUE_ATTRIBUTE("File",file);
					XML_VALUE_ATTRIBUTE("Priority",priority);

					packHierarchical[ priority ].push_back( file );
				}
			}

			XML_CHECK_NODE_FOR_EACH("Display")
			{
				XML_CHECK_VALUE_NODE("Width","Value",m_width);
				XML_CHECK_VALUE_NODE("Height","Value",m_height);
				XML_CHECK_VALUE_NODE("Bits","Value",m_bits);
				XML_CHECK_VALUE_NODE("FullScreen","Value",m_fullScreen);
			}

			XML_CHECK_VALUE_NODE("Game", "File", gameFile );
		}
	}
	XML_INVALID_PARSE()
	{
		//TODO: ERROR
		printf("parse application xml failed\n");
		return false;
	}

	printf("createDisplay [%d,%d] ...\n", m_width, m_height );

	HINSTANCE hInstance = GetModuleHandle(0);

	const char* className = "main window";

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= className;
	wcex.hIconSm		= 0;
	wcex.hIcon			= NULL; 
	RegisterClassEx(&wcex);

	RECT clientSize;

	clientSize.top = 0;
	clientSize.left = 0;
	clientSize.right = m_width;
	clientSize.bottom = m_height;

	DWORD style = WS_POPUP;

	if ( m_fullScreen == false )
	{
		style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	}

	AdjustWindowRect(&clientSize, style, FALSE);

	unsigned int realWidth = clientSize.right - clientSize.left;
	unsigned int realHeight = clientSize.bottom - clientSize.top;

	unsigned int windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
	unsigned int windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

	if ( m_fullScreen )
	{
		windowLeft = 0;
		windowTop = 0;
	}

	HWND hWnd = CreateWindow(className,"", style, windowLeft, windowTop, realWidth, realHeight,NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	MoveWindow(hWnd, windowLeft, windowTop, realWidth, realHeight, TRUE);

	renderInterface->createDisplay( hWnd, m_width, m_height, m_bits, m_fullScreen );

	inputInterface->init( hWnd, 0x00000003 );

	m_application->init();

	for each( const TPackHierarchical::value_type & it in packHierarchical )
	{
		for each( const std::string & pak in it.second )
		{
			m_application->loadPak( pak, it.first );
		}
	}

	m_application->createGame( gameFile );

	return true;
}
//////////////////////////////////////////////////////////////////////////
void WinApplication::run()
{
	MSG msg = MSG();
	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{	
			m_application->loop();
		}
	}
	
}