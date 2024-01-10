#define SDL_MAIN_HANDLED

#include "Environment/SDL/SDLIncluder.h"

#include "SDLApplication.h"

Mengine::SDLApplication MENGINE_application;

int SDL_main( int argc, char * argv[] )
{
    if( MENGINE_application.bootstrap( argc, argv ) == false )
    {
        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Mengine initialize", "Mengine invalid initialization", NULL );

        MENGINE_application.finalize();

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int SDL_loop(void)
{
    if( MENGINE_application.initialize() == false )
    {
        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Mengine initialize", "Mengine invalid initialization", NULL );

        MENGINE_application.finalize();

        return EXIT_FAILURE;
    }

    MENGINE_application.loop();

    MENGINE_application.finalize();

    return EXIT_SUCCESS;
}