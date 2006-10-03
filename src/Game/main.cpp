PREVED MEDVED
#	include "stdio.h"

#	include "../Menge/Application.h"

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