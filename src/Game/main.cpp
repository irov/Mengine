#	include "Interface/ApplicationInterface.h"

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

#	include <stdio.h>

#	include <vector>
#	include <string>
#	include <sstream>

float f (float x1, float x2)
{
	return 7 * x1 * x1 + 5 * x1 * x2 + x2*x2 + 6*x1 + 3*x2;
}

float f1(float x1, float x2)
{
	return 14 * x1 + 5*x2 + 6;
}

float f2(float x1, float x2)
{
	return 5 * x1 + 2*x2 + 3;
}

int main( int argc, char *argv[] )
{
	float res = f(1,-4);

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

	bool result = false;

//	result = app->init( "application.xml", "ReefLevel1");

	if( argc > 1 )
	{
		printf("%s", argv[1]);
		result = app->init( "application.xml", argv[1] );
	}
	else
	{
		result = app->init( "application.xml", 0 );
	}

	if( result == true )
	{
		app->run();
	}

	releaseInterfaceSystem( app );

	printf("free library '%s' \n", application_dll );
	FreeLibrary( hModule );

	return 1;
}