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

    bool initialize = app.initialize();

    if( initialize == true )
    {
        app.loop();
    }

    app.finalize();

    if( initialize == false )
    {
        ::MessageBoxA( NULL, "Mengine invalid initialization", "Mengine", MB_OK );

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}