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
	UNREFERENCED_PARAMETER( hInstance );
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );
	UNREFERENCED_PARAMETER( nShowCmd );
	
	stdex_allocator_initialize();

#	ifndef _DEBUG
	try
#	endif
	{
		Menge::WinApplication winApplication;

		bool initialize = winApplication.initialize();
    
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
#	ifndef _DEBUG
	catch( const std::exception & se )
	{		
		const char * se_what = se.what();

		MessageBoxA( NULL, se_what, "Mengine exception", MB_OK );
	}
#	endif

	stdex_allocator_finalize();
	    
	return 0;
}
//////////////////////////////////////////////////////////////////////////
