#include "UnixApplication.h"

#include "Kernel/Configuration.h"

#include "Config/StdString.h"

#include <cstdlib>

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
    Mengine::Configuration configuration;
    bool CLI = hasCLILaunchArgument( argc, argv );
    configuration.silentDialog = CLI;
    configuration.muteSound = CLI;

    Mengine::UnixApplication application;

    if( application.bootstrap( argc, argv, configuration ) == false )
    {
        application.finalize();

        return EXIT_FAILURE;
    }

    if( application.initialize() == false )
    {
        application.finalize();

        return EXIT_FAILURE;
    }

    application.loop();
    application.finalize();

    return EXIT_SUCCESS;
}
