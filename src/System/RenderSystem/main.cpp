#include "rendersystem.h"
#include <fstream>

RenderSystemInterface * RENDERSYSTEM;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;

	initInterfaceSystem(&RENDERSYSTEM);

	RENDERSYSTEM->createDisplay(1024,768,16,false);

	TextureDesc	td;

	std::ifstream	f("1.png",std::ios::binary);
	f.seekg(0,std::ios::end); 
	size_t filesize = f.tellg();
	f.seekg(0,std::ios::beg);
	char*	buffer = new char[filesize];
 	f.read(buffer,(int)filesize);
	f.close();

	td.buffer = buffer;
	td.size = filesize;
	td.filter = 1;
	
	RenderImageInterface* SPRITE0 = RENDERSYSTEM->loadImage(td);

	delete	buffer;

	f.open("2.bmp",std::ios::binary);
	f.seekg(0,std::ios::end); 
	filesize = f.tellg();
	f.seekg(0,std::ios::beg);
	buffer = new char[filesize];
 	f.read(buffer,(int)filesize);
	f.close();

	td.buffer = buffer;
	td.size = filesize;
	td.filter = 1;

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

			RENDERSYSTEM->beginSceneDrawing(0xFFFFFFFF);
				
			RENDERSYSTEM->renderImage(ident,mt::vec2f(0,0),mt::vec4f(0,0,1.0f,1.0f),mt::vec2f(SPRITE0->getWidth(),SPRITE0->getHeight()),0xFFFFFFFF,SPRITE0);
			RENDERSYSTEM->renderImage(ident,mt::vec2f(128,0),mt::vec4f(0,0,1.0f,1.0f),mt::vec2f(SPRITE0->getWidth(),SPRITE0->getHeight()),0xFFFFFFFF,SPRITE0);
			RENDERSYSTEM->renderImage(ident,mt::vec2f(256,0),mt::vec4f(0,0,1.0f,1.0f),mt::vec2f(SPRITE0->getWidth(),SPRITE0->getHeight()),0xFFFFFFFF,SPRITE0);
			RENDERSYSTEM->renderImage(ident,mt::vec2f(856,0),mt::vec4f(0,0,1.0f,1.0f),mt::vec2f(SPRITE0->getWidth(),SPRITE0->getHeight()),0xFFFFFFFF,SPRITE1);
			RENDERSYSTEM->renderImage(ident,mt::vec2f(896,0),mt::vec4f(0,0,1.0f,1.0f),mt::vec2f(SPRITE0->getWidth(),SPRITE0->getHeight()),0xFFFFFFFF,SPRITE1);
			RENDERSYSTEM->renderImage(ident,mt::vec2f(512,0),mt::vec4f(0,0,1.0f,1.0f),mt::vec2f(SPRITE0->getWidth(),SPRITE0->getHeight()),0xFFFFFFFF,SPRITE1);
			RENDERSYSTEM->renderImage(ident,mt::vec2f(356,0),mt::vec4f(0,0,1.0f,1.0f),mt::vec2f(SPRITE0->getWidth(),SPRITE0->getHeight()),0xFFFFFFFF,SPRITE0);
			
//			RENDERSYSTEM->renderImage(ident,mt::vec2f(128,0),mt::vec4f(0,0,1.0f,1.0f),mt::vec2f(SPRITE0->getWidth(),SPRITE0->getHeight()),0xFFFFFFFF,SPRITE0);

	
			
		
  		    RENDERSYSTEM->endSceneDrawing();
		}
    }

	RENDERSYSTEM->releaseRenderImage(SPRITE1);
	RENDERSYSTEM->releaseRenderImage(SPRITE0);
	releaseInterfaceSystem(RENDERSYSTEM);
    return 0;
}