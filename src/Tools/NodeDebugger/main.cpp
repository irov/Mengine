#include "NodeDebuggerApp.h"

#include "stdex/allocator.h"

int APIENTRY wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR lpCmdLine, int nShowCmd )
{
	(void)hInstance;
	(void)hPrevInstance;
	(void)lpCmdLine;
	(void)nShowCmd;

	stdex_allocator_initialize();

	{
		Mengine::NodeDebuggerApp app;

		if( app.Initialize() )
		{
			app.Loop();
			app.Shutdown();
		}
	}

	stdex_allocator_finalize();

    return 0;
}
