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

    bool initialize = application.initialize( argc, argv );

    if( initialize == true )
    {
        application.loop();
    }

    application.finalize();

    if( initialize == false )
    {
        SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Mengine initialize", "Mengine invalid initialization", NULL );

        return EXIT_FAILURE;
    }    

    return EXIT_SUCCESS;
}
