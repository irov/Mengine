#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Configuration.h"

#include "Win32Application.h"

#include "Config/Config.h"

//////////////////////////////////////////////////////////////////////////
//                          Entry point                                 //
//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    MENGINE_UNUSED( argc );
    MENGINE_UNUSED( argv );
    MENGINE_UNUSED( environ );

    Mengine::Win32Application app;

    Mengine::Configuration configuration;

    bool initialize = app.initialize( configuration );

    if( initialize == true )
    {
        app.loop();
    }
    else
    {
        if( configuration.silentDialog == false )
        {
            MessageBoxA( NULL, "Mengine invalid initialization", "Mengine", MB_OK );
        }
    }

    app.finalize();

    return EXIT_SUCCESS;
}