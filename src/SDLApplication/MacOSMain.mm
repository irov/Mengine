#include "Environment/SDL2/SDL2Includer.h"

#import "MengineAppleApplicationDelegates.h"

#include "SDLApplication.h"

int main( int argc, char * argv[] )
{
    NSArray * proxysClassed = getMengineAppleApplicationDelegates();

    for (NSString * className in proxysClassed) {
        Class c = NSClassFromString(@"MacOSSentryApplicationDelegate");

        if (c == nil) {
            continue;
        }
        
        id delegate = [c alloc];

        [delegate application];
    }
    
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

    return EXIT_SUCCESS;
}
