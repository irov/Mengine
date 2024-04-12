#include "Environment/SDL/SDLIncluder.h"

#import "Environment/MacOS/MacOSProxyApplicationDelegateInterface.h"

#import "MengineAppleApplicationDelegates.h"

#import <Foundation/Foundation.h>

#include "SDLApplication.h"

int main( int argc, char * argv[] )
{
    NSArray * proxysClassed = getMengineAppleApplicationDelegates();

    for (NSString * className in proxysClassed)
    {
        Class clazz = NSClassFromString(className);

        if( clazz == nil )
        {
            NSLog( @"invalid found application delegate %@"
              , className
            );
            
            return EXIT_FAILURE;
        }
        
        id<MacOSProxyApplicationDelegateInterface> delegate = [clazz alloc];

        if( [delegate application] == NO )
        {
            NSLog( @"invalid initialize application delegate %@"
              , className
            );
            
            return EXIT_FAILURE;
        }
    }
    
    Mengine::SDLApplication application;

    if( bootstrap.bootstrap( argc, argv ) == false )
    {
        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Mengine initialize", "Mengine invalid initialization", NULL );

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
