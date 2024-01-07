#define SDL_MAIN_HANDLED

#include "Environment/SDL/SDLIncluder.h"

#include "SDLApplication.h"

int SDL_main( int argc, char * argv[] )
{
    Mengine::SDLApplication application;

    bool initialize = application.initialize( argc, argv );

    if( initialize == false )
    {
        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Mengine initialize", "Mengine invalid initialization", NULL );

        application.finalize();

        return EXIT_FAILURE;
    }

    application.loop();

    application.finalize();

    return EXIT_SUCCESS;
}