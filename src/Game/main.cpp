#	include "Menge/Application.h"

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

#	include "Interface/ApplicationInterface.h"

void main()
{
#ifdef _DEBUG
	std::string str_systems = "Systems/WinApplication_d.dll";
#else
	std::string str_systems = "Systems/WinApplication.dll";
#endif

	HMODULE hInstance = LoadLibrary( str_systems.c_str() );

	printf("load library '%s'\n", str_systems.c_str() );
	
	typedef bool (*FInterfaceInitial)( ApplicationInterface **);
	typedef void (*FInterfaceRelease)( ApplicationInterface *);

	FInterfaceInitial init = (FInterfaceInitial)GetProcAddress( (HMODULE) hInstance, "initInterfaceSystem" );
	FInterfaceRelease fini = (FInterfaceRelease)GetProcAddress( (HMODULE) hInstance, "releaseInterfaceSystem" );

	ApplicationInterface * app = 0;

	init( &app );

	app->init( "application.xml" );

	app->run();

	fini( app );

	printf("free library '%s' \n", str_systems.c_str() );
	FreeLibrary( hInstance );
}