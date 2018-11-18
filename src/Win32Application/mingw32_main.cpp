#include "Environment/Windows/WindowsIncluder.h"

#include "Win32Application.h"

//////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    (void)argc;
    (void)argv;
    (void)environ;

    stdex_allocator_initialize();

#ifdef NDEBUG
    try
#endif
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
#ifdef NDEBUG
    catch( const std::exception & se )
    {
        const char * se_what = se.what();

        MessageBoxA( NULL, se_what, "Mengine exception", MB_OK );
    }
#endif

    stdex_allocator_finalize();

    return 0;
}