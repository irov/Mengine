#define SDL_MAIN_HANDLED

#include "Environment/SDL/SDLIncluder.h"

#include "SDLApplication.h"

int SDL_main( int argc, char * argv[] )
{
    Mengine::SDLApplication application;

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