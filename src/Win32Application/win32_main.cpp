#include "Environment/Windows/WindowsIncluder.h"

#include "Win32Application.h"

#include "Config/Config.h"

#include "stdex/allocator.h"

//////////////////////////////////////////////////////////////////////////
//                          Entry point                                 //
//////////////////////////////////////////////////////////////////////////
int APIENTRY WinMain( _In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ INT nShowCmd )
{
    MENGINE_UNUSED( hInstance );
    MENGINE_UNUSED( hPrevInstance );
    MENGINE_UNUSED( lpCmdLine );
    MENGINE_UNUSED( nShowCmd );

    stdex_allocator_initialize();

    {
        Mengine::Win32Application app;

        bool initialize = app.initialize();

        if( initialize == true )
        {
            app.loop();
        }
        else
        {
            ::MessageBoxA( NULL, "Mengine invalid initialization", "Mengine", MB_OK );
        }

        app.finalize();
    }

    stdex_allocator_finalize();

    return EXIT_SUCCESS;
}