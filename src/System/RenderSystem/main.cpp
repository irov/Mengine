#	include "vld.h"

#	include "d3d9RenderSystem.h"
#	include "..\..\Interface\RenderSystemInterface.h"
#	include <fstream>

mt::mat3f	ident;
RenderSystemInterface * renderSystem;
RenderImageInterface * SPRITE0;

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

INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
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

	int width = 1024;
	int height = 768;

	clientSize.top = 0;
	clientSize.left = 0;
	clientSize.right = width;
	clientSize.bottom = height;

	bool m_fullScreen = false;

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

	renderSystem = new Direct3d9RenderSystem();
	
	((Direct3d9RenderSystem*)renderSystem)->createDisplay(hWnd,width,height,16,false,true,false,true,false);

	TextureDesc	td;

	std::ifstream	f("1.jpg",std::ios::binary);
	f.seekg(0,std::ios::end); 
	size_t filesize = f.tellg();
	f.seekg(0,std::ios::beg);
	char*	buffer = new char[filesize];
 	f.read(buffer,(int)filesize);
	f.close();

	td.buffer = buffer;
	td.size = filesize;
	td.filter = 1;

	mt::ident_m3(ident);

	
	SPRITE0 = renderSystem->loadImage(td);

	delete[] buffer;

	MSG  msg;
    msg.message = WM_NULL;
    PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

    while( WM_QUIT != msg.message  )
    {
        bool bGotMsg = ( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) != 0 );

        if( bGotMsg )
        {
			 if( hWnd == NULL || 
                0 == TranslateAccelerator( hWnd, 0, &msg ) )
            
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
        }
        else
        {
			renderSystem->beginScene(2756);

			renderSystem->renderImage(
				ident,
				mt::vec2f(0,0),
				mt::vec4f(0,0,1.0f,1.0f),
				mt::vec2f(SPRITE0->getWidth(),SPRITE0->getHeight()),
				0xFFFFFFFF,
				SPRITE0);
	
			renderSystem->drawLine(mt::vec2f(0,0),mt::vec2f(100,100),0xFFFFaaFF);

			renderSystem->renderImage(
				ident,
				mt::vec2f(60,100),
				mt::vec4f(0,0,1.0f,1.0f),
				mt::vec2f(SPRITE0->getWidth(),SPRITE0->getHeight()),
				0xFFFFFFFF,
				SPRITE0);

			renderSystem->drawLine(mt::vec2f(400,0),mt::vec2f(300,900),0xFFFFFFAA);

			renderSystem->endScene();
        }
    }


	renderSystem->releaseRenderImage(SPRITE0);

	delete renderSystem;

	return 0;
}