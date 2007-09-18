#	include "Interface/ApplicationInterface.h"

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

#	include <stdio.h>

void main()
{

#ifdef _DEBUG
	const char * application_dll  = "Systems/WinApplication_d.dll";
#else
	const char * application_dll = "Systems/WinApplication.dll";
#endif

	HMODULE hInstance = LoadLibrary( application_dll );

	printf("load library '%s'\n", application_dll );
	
	typedef bool (*FInterfaceInitial)( ApplicationInterface **);
	typedef void (*FInterfaceRelease)( ApplicationInterface *);

	FInterfaceInitial initInterfaceSystem = 
		(FInterfaceInitial)GetProcAddress( (HMODULE) hInstance, "initInterfaceSystem" );

	FInterfaceRelease releaseInterfaceSystem = 
		(FInterfaceRelease)GetProcAddress( (HMODULE) hInstance, "releaseInterfaceSystem" );

	ApplicationInterface * app = 0;

	initInterfaceSystem( &app );

	if( app->init( "application.xml" ) )
	{
		app->run();
	}

	releaseInterfaceSystem( app );

	printf("free library '%s' \n", application_dll );
	FreeLibrary( hInstance );
}