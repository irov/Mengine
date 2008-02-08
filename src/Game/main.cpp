#	include "Interface/ApplicationInterface.h"

#	define WIN32_LEAN_AND_MEAN
#	define _WIN32_WINNT 0x0500
#	include <windows.h>

#	include <stdio.h>

#	include <vector>
#	include <string>
#	include <sstream>
#	include <io.h>

//void RedirectIOToConsole();
#	ifdef _CONSOLE
int main( int argc, char *argv[] )
#	elif
int APIENTRY WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
#	endif
{
#ifdef _DEBUG
	const char * application_dll  = "Systems_d/OgreApplication_d.dll";
#else
	const char * application_dll = "Systems/OgreApplication.dll";
#endif

#	ifndef _CONSOLE
	if( strstr(lpCmdLine, "-console") )
	{
		RedirectIOToConsole();
	}
#	endif
#ifndef MENGE_STATIC
	HMODULE hModule = LoadLibraryA( application_dll );

	if( hModule == 0 )
	{
		printf("Error: load library '%s' is failed\n", application_dll );
		return 0;
	}

	printf("load library '%s'\n", application_dll );
	
	typedef bool (*FInterfaceInitial)( ApplicationInterface **);
	typedef void (*FInterfaceRelease)( ApplicationInterface *);

	FInterfaceInitial initInterfaceSystem = 
		(FInterfaceInitial)GetProcAddress( (HMODULE) hModule, "initInterfaceSystem" );

	FInterfaceRelease releaseInterfaceSystem = 
		(FInterfaceRelease)GetProcAddress( (HMODULE) hModule, "releaseInterfaceSystem" );

	if( initInterfaceSystem == 0 || releaseInterfaceSystem == 0 )
	{
		printf("invalid open system application dll '%s'", application_dll );
		return 0;
	}
#endif

	ApplicationInterface * app = 0;

	if( initInterfaceSystem( &app ) == false )
	{
		printf("invalid init system application '%s'", application_dll );
		return 0;
	}

	bool result = false;

#	ifdef _CONSOLE
	if( argc > 0 )
	{
		result = app->init( "application.xml", argv[1] );
	}
	else
	{
		result = app->init( "application.xml", 0 );
	}
#	elif
	result = app->init( "application.xml", lpCmdLine );
#	endif

	if( result == true )
	{
		app->run();
	}

	releaseInterfaceSystem( app );

	printf("free library '%s' \n", application_dll );
	FreeLibrary( hModule );

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