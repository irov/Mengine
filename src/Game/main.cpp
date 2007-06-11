#	include "Menge/Application.h"
#	include "vld-10/vld.h"

void main()
{
	Menge::Application app;

	bool result = app.init("application.xml");

	if( result == false )
	{
		return;
	}

	app.run();
}