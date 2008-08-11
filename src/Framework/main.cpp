#	include "Framework.h"

#	include "..\Menge\LogEngine.h"

#	include <fstream>

Framework::Framework()
: platform(0)
//, m_logEngine(0)
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

void Framework::step()
{
	app->step();
}

void Framework::createWindow(HWND _hwnd)
{
	const char * config_file = "application.xml";

	Menge::ApplicationInterface * platform = 0;

	if( initInterfaceSystem( &platform ) == false )
	{
		assert(0);
	}

	bool result = false;
	app = new Menge::Application ( platform );

	Menge::LogSystemInterface * logSystem;
	initInterfaceSystem( &logSystem );
	app->setLogSystem( logSystem );

	Menge::ProfilerSystemInterface * profilerSystem;
	initInterfaceSystem( &profilerSystem );
	app->setProfilerSystem( profilerSystem );

	Menge::FileSystemInterface * fileSystem;
	initInterfaceSystem( &fileSystem );
	fileSystem->loadPath(".");
	app->setFileSystem( fileSystem );

	Menge::InputSystemInterface * inputSystem;
	initInterfaceSystem( &inputSystem );
	app->setInputSystem( inputSystem );


#	if	MENGE_PARTICLES	== (1)

	Menge::ParticleSystemInterface * particleSystem;
	initInterfaceSystem( &particleSystem );
	app->setParticleSystem( particleSystem );

#	endif

	Menge::PhysicSystem2DInterface * physicSystem2D;
	initInterfaceSystem( &physicSystem2D );
	app->setPhysicSystem2D( physicSystem2D );

	Menge::RenderSystemInterface * renderSystem;
	initInterfaceSystem( &renderSystem );
	app->setRenderSystem( renderSystem );


	Menge::SoundSystemInterface * soundSystem;
	initInterfaceSystem( &soundSystem );
	app->setSoundSystem( soundSystem );

	result = app->initialize( config_file, "" );
	result = app->createGame(_hwnd);

	/*if( result == true )
	{
		app.run();
	}

	std::ofstream file;
	file.open( "Hello.txt" );
	file << "I'm released!!";
	file.close();

	releaseInterfaceSystem( platform );

	releaseInterfaceSystem( soundSystem );
	releaseInterfaceSystem( renderSystem );
	releaseInterfaceSystem( physicSystem2D );
#	if	MENGE_PARTICLES	== (1)
	releaseInterfaceSystem( particleSystem );
#	endif
	releaseInterfaceSystem( inputSystem );
	releaseInterfaceSystem( fileSystem );
	releaseInterfaceSystem( logSystem );*/

	/*std::ofstream file;
	file.open( "Hello.txt" );
	file << "Hello";
	

	initInterfaceSystem( &platform );
	Menge::Application app( platform );

	Menge::RenderSystemInterface * renderSystem;
	initInterfaceSystem( &renderSystem );
	app.setRenderSystem( renderSystem );
	
	Menge::LogSystemInterface * logSystem;
	initInterfaceSystem( &logSystem );
	app.setLogSystem( logSystem );

	Menge::FileSystemInterface * fileSystem;
	initInterfaceSystem( &fileSystem );
	fileSystem->loadPath(".");
	app.setFileSystem( fileSystem );

	Menge::InputSystemInterface * inputSystem;
	initInterfaceSystem( &inputSystem );
	app.setInputSystem( inputSystem );

	platform->createWindow( _hwnd );

	m_renderEngine->initialize("");

	m_renderEngine->setContentResolution( mt::vec2f(1024,768) );

	m_renderEngine->createRenderWindow( 1024, 768, 32, false, _hwnd, 0, 0 );
	*/
	
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
	app.setSoundSystem( soundSystem );*/

	//m_renderEngine->createRenderWindow( 640, 480, 32, false, hwnd,
	//	0, 0 );

	
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