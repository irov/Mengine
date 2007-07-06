#include "d3d9RenderSystem.h"

#include "model.h"

Model *	pModel;

void	RenderScene()
{
	pModel->onUpdate(0.08);
	pModel->onRender();
}

INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
	Direct3d9RenderSystem * renderSystem = new Direct3d9RenderSystem();
	renderSystem->setRenderCallback(RenderScene);
	renderSystem->createDisplay(640,480,16,true);

	
 	pModel = new Model(renderSystem);

	pModel->setPath("..//..//..//dependencies//cal3d//data//cally//");
	pModel->onInit("..//..//..//dependencies//cal3d//data//cally//cally.cfg");

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

	delete pModel;

	delete renderSystem;

	return 0;
}