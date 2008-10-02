
#	include "Config/Config.h"

#	include "WinApplication.h"
#	include "Menge/Application.h"

#	if	MENGE_PARTICLES	== (1)
#		ifdef _DEBUG
#			pragma comment( lib, "AstralaxParticleSystem_d.lib" )
#		else
#			pragma comment( lib, "AstralaxParticleSystem.lib" )
#		endif
#	endif

//////////////////////////////////////////////////////////////////////////
//							Entry point									//
//////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain( __in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd )
{
	// keep compiler happy
	UNREFERENCED_PARAMETER( hInstance );
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( nShowCmd );

	Menge::WinApplication winApplication;
	const Menge::TCharA * config_file = "application.xml";

	Menge::Application app( &winApplication );

	if( app.initialize( config_file, lpCmdLine, true ) == false )
	{
		return 0;
	}

	if( app.createRenderWindow(0) == false )
	{
		return 0;
	}

	if( app.initGame() == false )
	{
		return 0;
	}

	app.run();

	return 0;
}
//////////////////////////////////////////////////////////////////////////