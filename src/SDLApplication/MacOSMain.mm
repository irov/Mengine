#include "Environment/SDL2/SDL2Includer.h"

#import "Environment/MacOS/MacOSProxyApplicationDelegateInterface.h"

#import "MengineAppleApplicationDelegates.h"

#include "SDLApplication.h"

int main( int argc, char * argv[] )
{
    NSArray * proxysClassed = getMengineAppleApplicationDelegates();

    for (NSString * className in proxysClassed)
    {
        Class clazz = NSClassFromString(className);

        if( clazz == nil )
        {
            return EXIT_FAILURE;
        }
        
        id<MacOSProxyApplicationDelegateInterface> delegate = [clazz alloc];

        if( [delegate application] == NO )
        {
            return EXIT_FAILURE;
        }
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
