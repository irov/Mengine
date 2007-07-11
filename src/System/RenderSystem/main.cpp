//#	include "vld.h"

#	include "d3d9RenderSystem.h"
#	include "..\..\Interface\RenderSystemInterface.h"
#	include "model.h"

Model	pModel;
mt::mat3f	ident;
RenderSystemInterface *renderSystem;
RenderImageInterface* SPRITE0;

void	RenderScene()
{
	/*renderSystem->renderImage(
		ident,
		mt::vec2f(128,0),
		mt::vec4f(0,0,1.0f,1.0f),
		mt::vec2f(SPRITE0->getWidth(),
		SPRITE0->getHeight()),
		0xFFFFFFFF,
		SPRITE0);
*/
	pModel.onIdle(0.001f);
	pModel.onRender();

/*	renderSystem->renderImage(
		ident,
		mt::vec2f(0,0),
		mt::vec4f(0,0,1.0f,1.0f),
		mt::vec2f(SPRITE0->getWidth(),SPRITE0->getHeight()),
		0xFFFFFFFF,
		SPRITE0);

*/
}

INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
	renderSystem = new Direct3d9RenderSystem();
	renderSystem->setRenderCallback(RenderScene);
	renderSystem->createDisplay(640,480,16,false);
/*
	TextureDesc	td;

	std::ifstream	f("HLSLwithoutEffects.jpg",std::ios::binary);
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
*/
 	pModel.onInit(renderSystem,"heroine.cfg");

	MSG  msg;
    msg.message = WM_NULL;
    PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );

	HWND hWnd = GetActiveWindow();

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
			renderSystem->update();
        }
    }

	pModel.onShutdown();
	
	renderSystem->releaseRenderImage(SPRITE0);

	delete renderSystem;

	return 0;
}