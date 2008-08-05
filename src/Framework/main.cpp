#	include "Framework.h"


Framework::Framework()
: platform(0)
{
}

Framework::~Framework()
{
	//releaseInterfaceSystem( platform );
}

/*long Framework::createWindow(char * _title, int _width, int _height, bool _isfullscreen)
{
	void * winHandle = platform->createWindow( _title, _width, _height, _isfullscreen );
	return (long)winHandle;
}*/

void Framework::createWindow(HWND hwnd)
{
	initInterfaceSystem( &platform );
	//Menge::Application app( platform );

	Menge::RenderSystemInterface * renderSystem;
	initInterfaceSystem( &renderSystem );
	
	m_renderEngine =  new Menge::RenderEngine( renderSystem );

	//Menge::Holder<Menge::RenderEngine>::keep( m_renderEngine );

/*
	platform->createWindow( hwnd );*/
	

/*	Menge::LogSystemInterface * logSystem;
	initInterfaceSystem( &logSystem );
	app.setLogSystem( logSystem );

	Menge::FileSystemInterface * fileSystem;
	initInterfaceSystem( &fileSystem );
	fileSystem->loadPath(".");
	app.setFileSystem( fileSystem );

	Menge::InputSystemInterface * inputSystem;
	initInterfaceSystem( &inputSystem );
	app.setInputSystem( inputSystem );

	Menge::PhysicSystem2DInterface * physicSystem2D;
	initInterfaceSystem( &physicSystem2D );
	app.setPhysicSystem2D( physicSystem2D );

	Menge::SoundSystemInterface * soundSystem;
	initInterfaceSystem( &soundSystem );
	app.setSoundSystem( soundSystem );
*/
	
	/*Menge::RenderSystemInterface * renderSystem;
	initInterfaceSystem( &renderSystem );
	app.setRenderSystem( renderSystem );*/

	///m_renderEngine->createRenderWindow( 640, 480, 32, false, hwnd,
	//	0, 0 );

	//printf("1!\n");
}

/*extern "C" EXAMPLEUNMANAGEDDLL_API void MengeInit()
{
	printf("init Menge!\n");
	if(initInterfaceSystem( &platform ) == false)
	{
		printf("ERROR!\n");
	}
}

extern "C" EXAMPLEUNMANAGEDDLL_API void MengeFree()
{
	releaseInterfaceSystem( platform );
}

extern "C" EXAMPLEUNMANAGEDDLL_API long int MengeCreateWindow(char * _title, int _width, int _height, bool _isfullscreen)
{
	printf("title!\n");
	void * winHandle = platform->createWindow( _title, _width, _height, _isfullscreen );
	return 0;//(long int)winHandle;
}*/

BOOL APIENTRY DllMain(
					  HMODULE,
					  DWORD ul_reason_for_call,
					  LPVOID)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}