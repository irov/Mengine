
#	include "Config/Config.h"

#	include "WinApplication.h"

//////////////////////////////////////////////////////////////////////////
//							Entry point									//
//////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{ 
	// keep compiler happy
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( nShowCmd );

	Menge::WinApplication winApplication( hInstance, lpCmdLine );
	
	if( winApplication.initialize() == true )
	{
		winApplication.loop();
	}
	
	winApplication.finalize();

	return 0;
}
//////////////////////////////////////////////////////////////////////////
