//#	include <vld.h>

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

	//_CrtSetDbgFlag(
	//	_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) |
	//	_CRTDBG_CHECK_ALWAYS_DF |
	//	_CRTDBG_DELAY_FREE_MEM_DF |
	//	_CRTDBG_LEAK_CHECK_DF
	//	);

	//_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
	//_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);

	Menge::WinApplication winApplication;
	
	bool initialize = winApplication.initialize( hInstance, lpCmdLine );
    
    if( initialize == true )
	{
        winApplication.loop();
    }
    
	winApplication.finalize();
    
	return 0;
}
//////////////////////////////////////////////////////////////////////////
