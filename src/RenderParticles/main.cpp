#include "RenderParticlesSuites.h"
//////////////////////////////////////////////////////////////////////////
//							Entry point									//
//////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd )
{ 
	// keep compiler happy
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( nShowCmd );
	
    Menge::RenderParticlesSuites sandbox;
	if( sandbox.initialise( hInstance, 800, 600 ) == false )
	{
		while(true){}
		return -1;
	}
	sandbox.loop();

	sandbox.destroy();

	return 0;
}
//////////////////////////////////////////////////////////////////////////
