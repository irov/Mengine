#include "SDL_main.h"

#include <wrl.h>

#ifdef _MSC_VER
#pragma warning(disable:4447)
#endif

#ifdef _MSC_VER
#pragma comment(lib, "runtimeobject.lib")
#endif

int CALLBACK WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
    return SDL_WinRTRunApp( &SDL_main, NULL );
}
