#	include "Interface/ApplicationInterface.h"

#	include "Interface/LogSystemInterface.h"
#	include "Interface/ProfilerSystemInterface.h"
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

#	include <string>
#	include <sstream>
#	include <io.h>
#	include <iosfwd>

#	include "Config\Config.h"
#	include "..\Menge\Application.h"


#	if	MENGE_PARTICLES	== (1)
#		ifdef _DEBUG
#			pragma comment( lib, "AstralaxParticleSystem_d.lib" )
#		else
#			pragma comment( lib, "AstralaxParticleSystem.lib" )
#		endif
#	endif

class PlatformHolder
{
public:
	PlatformHolder()
		: m_platform(0)
	{

	}

	~PlatformHolder()
	{
		releaseInterfaceSystem( m_platform );
	}

	Menge::ApplicationInterface * initialize()
	{
		if( initInterfaceSystem( &m_platform ) == false )
		{
			printf("invalid init system application \n" );
		}

		return m_platform;
	}

protected:
	Menge::ApplicationInterface * m_platform;
};

void RedirectIOToConsole();
int APIENTRY WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
{
	// keep compiler happy
	UNREFERENCED_PARAMETER( hInstance );
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( nShowCmd );

	const Menge::TChar * config_file = MENGE_TEXT("application.xml");

	if( strstr(lpCmdLine, "-console") )
	{
		RedirectIOToConsole();
	}

	PlatformHolder platform;

	Menge::ApplicationInterface * platformInterface = platform.initialize();

	Menge::Application app( platformInterface );

	if( app.initialize( config_file, lpCmdLine ) == false )
	{
		return 0;
	}

	if( app.loadGame() == false )
	{
		return 0;
	}

	if( app.createRenderWindow(0) == false )
	{
		return 0;
	}

	if( app.initGame(true) == false )
	{
		return 0;
	}

	app.run();

	if( strstr(lpCmdLine, "-console") )
	{
		FreeConsole();
	}

	return 0;
}

void RedirectIOToConsole()
{
	int hConHandle;
	HANDLE lStdHandle;
	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	FILE *fp;
	// try to attach to calling console first
	BOOL b = AttachConsole( ATTACH_PARENT_PROCESS );
	// allocate a console for this app
	if( !b )
	{
		AllocConsole();
		// set the screen buffer to be big enough to let us scroll text
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
		coninfo.dwSize.Y = 1000;
		SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);
	}
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
	std::ios::sync_with_stdio();
}