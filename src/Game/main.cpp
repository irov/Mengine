#	include "Interface/ApplicationInterface.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/FileSystemInterface.h"
#	include "Interface/InputSystemInterface.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Interface/SoundSystemInterface.h"
#	include "Interface/ParticleSystemInterface.h"
#	include "Interface/PhysicSystemInterface.h"
#	include "Interface/PhysicSystem2DInterface.h"


#	define WIN32_LEAN_AND_MEAN
#	define _WIN32_WINNT 0x0500
#	include <windows.h>

#	include <stdio.h>

//#	include <vector>
#	include <string>
#	include <sstream>
#	include <io.h>

#	include "Config\Config.h"
#	include "..\Menge\Application.h"

//#define API_PLATFORM	"WinApplication"


#	if	MENGE_PARTICLES	== (1)
#		ifdef _DEBUG
#			pragma comment( lib, "AstralaxParticleSystem_d.lib" )
#		else
#			pragma comment( lib, "AstralaxParticleSystem.lib" )
#		endif
#	endif


#	ifdef _CONSOLE
int main( int argc, char *argv[] )
#	else
void RedirectIOToConsole();
int APIENTRY WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
#	endif
{
#ifndef _CONSOLE
	// keep compiler happy
	UNREFERENCED_PARAMETER( hInstance );
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( nShowCmd );
#endif	

	const char * config_file = "application.xml";
#	ifndef _CONSOLE
	if( strstr(lpCmdLine, "-console") )
	{
		RedirectIOToConsole();
	}
#	endif

	ApplicationInterface * platform = 0;

	if( initInterfaceSystem( &platform ) == false )
	{
		printf("invalid init system application \n" );
		return 0;
	}

	bool result = false;
	Menge::Application app( platform );

	FileSystemInterface * fileSystem;
	initInterfaceSystem( &fileSystem );
	fileSystem->loadPath(".");
	app.setFileSystem( fileSystem );

	InputSystemInterface * inputSystem;
	initInterfaceSystem( &inputSystem );
	app.setInputSystem( inputSystem );
	
	LogSystemInterface * logSystem;
	initInterfaceSystem( &logSystem );
	app.setLogSystem( logSystem );

#	if	MENGE_PARTICLES	== (1)

	ParticleSystemInterface * particleSystem;
	initInterfaceSystem( &particleSystem );
	app.setParticleSystem( particleSystem );

#	endif

	PhysicSystem2DInterface * physicSystem2D;
	initInterfaceSystem( &physicSystem2D );
	app.setPhysicSystem2D( physicSystem2D );

	RenderSystemInterface * renderSystem;
	initInterfaceSystem( &renderSystem );
	app.setRenderSystem( renderSystem );

	SoundSystemInterface * soundSystem;
	initInterfaceSystem( &soundSystem );
	app.setSoundSystem( soundSystem );

	/*PhysicSystemInterface * physicSystem3D;
	initInterfaceSystem( &physicSystem3D );
	app.setPhysicSystem( physicSystem3D );*/
	
#	ifdef _CONSOLE
	if( argc > 1 )
	{
		char* cmdline = 0;
		unsigned int l = 0;
		for ( int i = 1; i != argc; ++i )
		{
			l += (unsigned int)strlen( argv[i] );
		}
		cmdline = new char[l+1];
		cmdline[0] = '\0';
		for ( int i = 1; i < argc; i++ )
		{
			strcat_s( cmdline, l+1, argv[i] );
		}
		//result = app->init( "application.xml", cmdline );
		result = app.initialize( config_file, cmdline );
		delete[] cmdline;
	}
	else
	{
		//result = app->init( "application.xml", 0 );
		result = app.initialize( config_file, "" );
	}
#	else
	//Menge::Application app( platform );

	result = app.initialize( config_file, lpCmdLine );
	//result = app->init( config_file, lpCmdLine );
#	endif

	if( result == true )
	{
		app.run();
	}

	releaseInterfaceSystem( platform );

	releaseInterfaceSystem( soundSystem );
	releaseInterfaceSystem( renderSystem );
	releaseInterfaceSystem( physicSystem2D );
#	if	MENGE_PARTICLES	== (1)
	releaseInterfaceSystem( particleSystem );
#	endif
	releaseInterfaceSystem( logSystem );
	releaseInterfaceSystem( inputSystem );
	releaseInterfaceSystem( fileSystem );
	
	return 0;
}

void RedirectIOToConsole()
{
	int hConHandle;
	HANDLE lStdHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;
	//BOOL b = AttachConsole( ATTACH_PARENT_PROCESS );
	
	// allocate a console for this app
	AllocConsole();
	// set the screen buffer to be big enough to let us scroll text
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
	coninfo.dwSize.Y = 1000;
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
	// redirect unbuffered STDOUT to the console
	lStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle((intptr_t)lStdHandle, 0x4000);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );
	// redirect unbuffered STDIN to the console
	lStdHandle = GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle((intptr_t)lStdHandle, 0x4000);
	fp = _fdopen( hConHandle, "r" );
	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );
	// redirect unbuffered STDERR to the console
	lStdHandle = GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle((intptr_t)lStdHandle, 0x4000);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );
	//::MoveWindow( GetConsoleWindow(), 0, 650, 0, 0, TRUE );
	// make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
	// point to console as well
	//     ios::sync_with_stdio();
}