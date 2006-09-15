#	include "stdio.h"

#	include "../Menge/SceneManager.h"

void main()
{
	RvEngine::SceneManager sm;

	bool result = sm.init("Game_d.xml");

	if( result == false )
	{
		return;
	}

	bool display = sm.createDisplay(1024,768,32,false);

	std::string bubaPath = sm.getPath("Objects","Buba");

	printf("%d - %s\n",result?1:0,bubaPath.c_str());

	sm.run();
}