#include "Environment/SDL/SDLIncluder.h"

#include "SDLApplication.h"

int main( int argc, char * argv[] )
{
    Mengine::SDLApplication application;

    if( application.bootstrap( argc, argv ) == false )
    {
        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Mengine initialize", "Mengine invalid bootstrap", NULL );

        application.finalize();

        return EXIT_FAILURE;
    }

    if( application.initialize() == false )
    {
        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Mengine initialize", "Mengine invalid initialize", NULL );

        application.finalize();

        return EXIT_FAILURE;
    }

    application.loop();

    application.finalize();

    return EXIT_SUCCESS;
}