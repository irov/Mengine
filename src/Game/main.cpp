#	include "Menge/Application.h"
#	include "vld/vld.h"

void main()
{
	Menge::Application app;

	printf("Application init ...\n");
	bool result = app.init("application.xml");

	if( result == false )
	{
		printf("Application not init\n");
		return;
	}

	printf("Application run ...\n");
	app.run();
}