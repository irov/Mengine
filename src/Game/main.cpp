#	include "../Menge/Application.h"
//#	include "../Menge/vld.h"

#	include <iostream>
#	include <list>
void main()
{
	std::list<int>  t;

	t.push_back(44);
	t.push_back(78);
	t.push_back(89);

	for (std::list<int>::iterator i = t.begin(); i != t.end();  ++i)
	{
		int u = 0;
	}

//
	//--it;

	//t.push_back(14);

	Menge::Application app;

	bool result = app.init("application.xml");

	if( result == false )
	{
		return;
	}

	app.run();
}