#	include "WIN32/WindowsIncluder.h"

#	include "Win32Application.h"

//////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[], char *environ )
{
    (void)argc;
    (void)argv;
    (void)environ;

    stdex_allocator_initialize();

    try
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
    catch( const std::exception & se )
    {
        const char * se_what = se.what();

        MessageBoxA( NULL, se_what, "Mengine exception", MB_OK );
    }

    stdex_allocator_finalize();
}