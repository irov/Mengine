#include "UnixApplication.h"

#include <cstdlib>

//////////////////////////////////////////////////////////////////////////
//                          Entry point                                 //
//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    Mengine::UnixApplication application;

    if( application.bootstrap( argc, argv ) == false )
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
