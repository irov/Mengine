#include "Environment/Windows/WindowsIncluder.h"

#include "Win32Application.h"

#include "Config/Config.h"

//////////////////////////////////////////////////////////////////////////
//							Entry point									//
//////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    MENGINE_UNUSED( argc );
    MENGINE_UNUSED( argv );
    MENGINE_UNUSED( environ );

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
            MessageBoxA( NULL, "Mengine invalid initialization", "Mengine", MB_OK );
        }

        app.finalize();
    }

    stdex_allocator_finalize();

    return EXIT_SUCCESS;
}