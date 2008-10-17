
#	include "Config/Config.h"

#	include "SDLApplication.h"

#	if	MENGE_PARTICLES	== (1)
#		ifdef _DEBUG
#			pragma comment( lib, "AstralaxParticleSystem_d.lib" )
#		else
#			pragma comment( lib, "AstralaxParticleSystem.lib" )
#		endif
#	endif

#	include <Windows.h>
//////////////////////////////////////////////////////////////////////////
//							Entry point									//
//////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
{
	UNREFERENCED_PARAMETER( hInstance );
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( nShowCmd );

	Menge::SDLApplication app( lpCmdLine );
	if( app.start() == false )
	{
		return 0;
	}
	app.loop();

	return 0;
}
//////////////////////////////////////////////////////////////////////////