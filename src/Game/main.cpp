#	include "Interface/ApplicationInterface.h"

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

#	include <stdio.h>


int main()
{
	int * t = NULL;
	delete t;
#ifdef _DEBUG
	const char * application_dll  = "Systems_d/OgreApplication_d.dll";
#else
	const char * application_dll = "Systems/OgreApplication.dll";
#endif

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

	if( app->init( "application.xml" ) )
	{
		app->run();
	}

	releaseInterfaceSystem( app );

	printf("free library '%s' \n", application_dll );
	FreeLibrary( hModule );

	return 1;
}