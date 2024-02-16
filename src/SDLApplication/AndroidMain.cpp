#define SDL_MAIN_HANDLED

#include "Environment/SDL/SDLIncluder.h"

#include "SDLApplication.h"

#include "Config/StdLib.h"

Mengine::SDLApplication * MENGINE_application;

int SDL_main( int argc, char * argv[] )
{
    if( MENGINE_application == nullptr )
    {
        MENGINE_application = new Mengine::SDLApplication;
    }
    else
    {
        return EXIT_FAILURE;
    }

    if( MENGINE_application == nullptr )
    {
        return EXIT_FAILURE;
    }

    if( MENGINE_application->bootstrap( argc, argv ) == false )
    {
        MENGINE_application->finalize();

        delete MENGINE_application;

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int SDL_loop(void)
{
    if( MENGINE_application == nullptr )
    {
        return EXIT_FAILURE;
    }

    if( MENGINE_application->initialize() == false )
    {
        MENGINE_application->finalize();

        delete MENGINE_application;

        return EXIT_FAILURE;
    }

    MENGINE_application->loop();

    MENGINE_application->finalize();

    delete MENGINE_application;
    MENGINE_application = nullptr;

    return EXIT_SUCCESS;
}