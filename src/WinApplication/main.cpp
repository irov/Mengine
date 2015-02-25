#	include "WindowsLayer/WindowsIncluder.h"

#	include "Config/Config.h"

#	include "WinApplication.h"

#	include "Interface/ServiceInterface.h"

//////////////////////////////////////////////////////////////////////////
//							Entry point									//
//////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{ 
	// keep compiler happy
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( nShowCmd );

	Menge::WinApplication winApplication;

	try
	{
		bool initialize = winApplication.initialize( hInstance, lpCmdLine );
    
		if( initialize == true )
		{
			winApplication.loop();
		}
		else
		{
			MessageBoxA( NULL, "Mengine invalid initialization", "Mengine", MB_OK );
		}
    
		winApplication.finalize();
	}
	catch( const std::exception & se )
	{		
		const char * se_what = se.what();

		MessageBoxA( NULL, se_what, "Mengine exception", MB_OK );
	}
    
	return 0;
}
//////////////////////////////////////////////////////////////////////////
