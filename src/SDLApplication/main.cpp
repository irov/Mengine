#include "SDLApplication.h"

#include "Config/Config.h"

#include "SDL.h"

#include "stdex/allocator.h"

//////////////////////////////////////////////////////////////////////////
//                          Entry point                                 //
//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    stdex_allocator_initialize();

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
    }

    stdex_allocator_finalize();

    return 0;
}
//////////////////////////////////////////////////////////////////////////
