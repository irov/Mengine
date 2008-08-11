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

void Framework::step(float _timing)
{
	app->step(_timing);
}

void Framework::createWindow(HWND _hwnd)
{
	const char * config_file = "application.xml";

	Menge::ApplicationInterface * platform = 0;

	if( initInterfaceSystem( &platform ) == false )
	{
		assert(0);
	}

	app = new Menge::Application( platform );

	Menge::LogSystemInterface * logSystem;
	initInterfaceSystem( &logSystem );
	app->setLogSystem( logSystem );

	Menge::ProfilerSystemInterface * profilerSystem;
	initInterfaceSystem( &profilerSystem );
	app->setProfilerSystem( profilerSystem );

	Menge::FileSystemInterface * fileSystem;
	initInterfaceSystem( &fileSystem );
	app->setFileSystem( fileSystem );

	fileSystem->loadPath(".");

	Menge::InputSystemInterface * inputSystem;
	initInterfaceSystem( &inputSystem );
	app->setInputSystem( inputSystem );

	Menge::ParticleSystemInterface * particleSystem;
	initInterfaceSystem( &particleSystem );
	app->setParticleSystem( particleSystem );

	Menge::PhysicSystem2DInterface * physicSystem2D;
	initInterfaceSystem( &physicSystem2D );
	app->setPhysicSystem2D( physicSystem2D );

	Menge::RenderSystemInterface * renderSystem;
	initInterfaceSystem( &renderSystem );
	app->setRenderSystem( renderSystem );

	Menge::SoundSystemInterface * soundSystem;
	initInterfaceSystem( &soundSystem );
	app->setSoundSystem( soundSystem );

	bool result = app->initialize( config_file, "" );
	result = app->createGame(_hwnd);

	/*if( result == true )
	{
		app.run();
	}

	releaseInterfaceSystem( platform );

	releaseInterfaceSystem( soundSystem );
	releaseInterfaceSystem( renderSystem );
	releaseInterfaceSystem( physicSystem2D );
	releaseInterfaceSystem( particleSystem );
	releaseInterfaceSystem( inputSystem );
	releaseInterfaceSystem( fileSystem );
	releaseInterfaceSystem( logSystem );*/
}


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