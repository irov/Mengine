#include "SDLApplication.h"

#include "SDL.h"

//////////////////////////////////////////////////////////////////////////
//                          Entry point                                 //
//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    Mengine::SDLApplication application;

    bool initialize = application.initialize( argc, argv );

    if( initialize == true )
    {
        application.loop();
    }
    else
    {
        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Mengine initialize", "Mengine invalid initialization", NULL );
    }

    application.finalize();

    return 0;
}
//////////////////////////////////////////////////////////////////////////
