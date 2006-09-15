#include "rendersystem.h"
#include <fstream>

RenderSystemInterface * RENDERSYSTEM;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	initRenderSystem(&RENDERSYSTEM);

	RENDERSYSTEM->createDisplay(1024,768,16,false);

	TextureDesc	td;

	std::ifstream	f("1.png",std::ios::binary);
	f.seekg(0,std::ios::end); 
	size_t filesize = f.tellg();
	f.seekg(0,std::ios::beg);
	char*	buffer = new char[filesize];
 	f.read(buffer,filesize);
	f.close();

	td.buffer = buffer;
	td.size = filesize;
	td.haveAlpha = false;
	
	RenderImageInterface* SPRITE0 = RENDERSYSTEM->loadImage(td);

	delete	buffer;

	f.open("2.bmp",std::ios::binary);
	f.seekg(0,std::ios::end); 
	filesize = f.tellg();
	f.seekg(0,std::ios::beg);
	buffer = new char[filesize];
 	f.read(buffer,filesize);
	f.close();

	td.buffer = buffer;
	td.size = filesize;
	td.haveAlpha = true;

	RenderImageInterface* SPRITE1 = RENDERSYSTEM->loadImage(td);

	delete	buffer;
	
    ZeroMemory( &msg, sizeof(msg) );


	mt::mat3f	s_ident;
	mt::ident_m3(s_ident);

    while( msg.message!=WM_QUIT )
    {
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
			TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
		{
			mt::mat3f	ident;
			mt::ident_m3(ident);

			RENDERSYSTEM->beginSceneDrawing(true,false,0xFFFFFFFF);
				
			RENDERSYSTEM->renderImage(ident,0xFFFFFFFF,SPRITE0);


		//	s_ident.v0.x = 1.8f;
		//	s_ident.v0.y= 0.3;

		//	s_ident.v1.x = 1.0f;

		//	s_ident.v0.z+=0.4f;
			s_ident.v0.z+=0.4f;
		
			RENDERSYSTEM->renderImage(s_ident,0xAACCCCFF,SPRITE1);
		
  		    RENDERSYSTEM->endSceneDrawing();
		}
    }

	RENDERSYSTEM->releaseRenderImage(SPRITE1);
	RENDERSYSTEM->releaseRenderImage(SPRITE0);
	releaseRenderSystem(RENDERSYSTEM);
    return 0;
}