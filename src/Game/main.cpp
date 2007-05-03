#	include "../Menge/Application.h"
//#	include "../Menge/vld.h"

#	include <iostream>
#	include <list>

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