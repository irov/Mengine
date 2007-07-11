#	include "Menge/Application.h"
#	include "vld/vld.h"

#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>

#	include "Interface/ApplicationInterface.h"

void main()
{
	HINSTANCE m_hInstance = LoadLibrary( "Systems/WinApplication_d.dll" );

	typedef bool (*FInterfaceInitial)( ApplicationInterface **);
	typedef void (*FInterfaceRelease)( ApplicationInterface *);

	FInterfaceInitial m_init = (FInterfaceInitial)GetProcAddress( (HMODULE) m_hInstance, "initInterfaceSystem" );
	FInterfaceRelease m_fini = (FInterfaceRelease)GetProcAddress( (HMODULE) m_hInstance, "releaseInterfaceSystem" );

	ApplicationInterface * app = 0;

	m_init( &app );

	app->init( "application.xml" );

	app->run();


	m_fini( app );
	//Menge::Application app;

	//printf("Application init ...\n");
	//bool result = app.init("application.xml");

	//if( result == false )
	//{
	//	printf("Application not init\n");
	//	return;
	//}

	//printf("Application run ...\n");
	//app.run();
}