#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/Configuration.h"

#include "Win32Application.h"

#include "Config/Config.h"
#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
static bool hasCLILaunchArgument( int _argc, char * _argv[] )
{
    for( int index = 1; index != _argc; ++index )
    {
        if( Mengine::StdString::strcmp( _argv[index], "--cli" ) == 0 )
        {
            return true;
        }
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
//                          Entry point                                 //
//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    MENGINE_UNUSED( environ );

    Mengine::Win32Application app;

    Mengine::Configuration configuration;
    bool CLI = hasCLILaunchArgument( argc, argv );
    configuration.silentDialog = CLI;
    configuration.muteSound = CLI;

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
