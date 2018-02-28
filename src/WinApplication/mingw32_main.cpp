#	include "WIN32/WindowsIncluder.h"

#	include "WinApplication.h"

//////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[], char *environ )
{
    (void)argc;
    (void)argv;
    (void)environ;

    stdex_allocator_initialize();

    try
    {
        Menge::WinApplication winApplication;

        bool initialize = winApplication.initialize();

        if( initialize == true )
        {
            winApplication.loop();
        }
        else
        {
            MessageBoxA( NULL, "Mengine invalid initialization", "Mengine", MB_OK );
        }

        winApplication.finalize();
    }
    catch( const std::exception & se )
    {
        const char * se_what = se.what();

        MessageBoxA( NULL, se_what, "Mengine exception", MB_OK );
    }

    stdex_allocator_finalize();
}