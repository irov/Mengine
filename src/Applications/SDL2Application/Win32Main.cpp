#include "Environment/SDL2/SDL2MainIncluder.h"

#include "SDL2Application.h"

#include "Config/StdLib.h"

int SDL_main( int argc, char * argv[] )
{
    Mengine::SDL2Application application;

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