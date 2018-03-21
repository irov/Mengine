#   include "Config/Config.h"

#   include "SDLApplication.h"

#   include "Interface/ServiceInterface.h"

#   include "SDL.h"

//////////////////////////////////////////////////////////////////////////
//							Entry point									//
//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    stdex_allocator_initialize();

    Mengine::SDLApplication application;

#	ifndef _DEBUG
    try
#   endif
    {
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
#	ifndef _DEBUG
    catch( const std::exception & se )
    {
        const char * se_what = se.what();

        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Mengine exception", se_what, NULL );
    }
#   endif

    return 0;
}
//////////////////////////////////////////////////////////////////////////
