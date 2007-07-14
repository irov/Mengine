#	include "Menge/Application.h"
//#	include "vld/vld.h"

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

#	include "Interface/ApplicationInterface.h"

void main()
{
	HINSTANCE hInstance = LoadLibrary( "Systems/WinApplication_d.dll" );

	typedef bool (*FInterfaceInitial)( ApplicationInterface **);
	typedef void (*FInterfaceRelease)( ApplicationInterface *);

	FInterfaceInitial init = (FInterfaceInitial)GetProcAddress( (HMODULE) hInstance, "initInterfaceSystem" );
	FInterfaceRelease fini = (FInterfaceRelease)GetProcAddress( (HMODULE) hInstance, "releaseInterfaceSystem" );

	ApplicationInterface * app = 0;

	init( &app );

	app->init( "application.xml" );

	app->run();

	fini( app );

	FreeLibrary( hInstance );
}