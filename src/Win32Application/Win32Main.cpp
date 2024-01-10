#include "Environment/Windows/WindowsIncluder.h"

#include "Win32Application.h"

//////////////////////////////////////////////////////////////////////////
//                          Entry point                                 //
//////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain( _In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ INT nShowCmd )
{
    MENGINE_UNUSED( hInstance );
    MENGINE_UNUSED( hPrevInstance );
    MENGINE_UNUSED( lpCmdLine );
    MENGINE_UNUSED( nShowCmd );

    Mengine::Win32Application app;

    if( app.initialize() == false )
    {
        ::MessageBoxA( NULL, "Mengine invalid initialization", "Mengine", MB_OK );

        app.finalize();

        return EXIT_FAILURE;
    }

    app.loop();

    app.finalize();

    return EXIT_SUCCESS;
}