#include "Environment/SDL2/SDL2Includer.h"

#import "Environment/MacOS/MacOSProxyApplicationDelegateInterface.h"
#import "Environment/MacOS/MacOSApplication.h"

#import "MacOSApplicationDelegates.h"

#import <Foundation/Foundation.h>

#include "SDL2Application.h"

int main( int argc, char * argv[] )
{
    NSLog(@"🟢 Launch Megnine application");

    [MacOSApplication.sharedInstance initialize];

    NSArray * proxysClassed = [MacOSApplicationDelegates getApplicationDelegates];

    for( NSString * className in proxysClassed )
    {
        Class clazz = NSClassFromString(className);

        if( clazz == nil )
        {
            NSLog( @"🔴 [ERROR] Invalid found application delegate: %@"
                , className
            );

            return EXIT_FAILURE;
        }

        id<MacOSProxyApplicationDelegateInterface> delegate = [clazz alloc];

        if( [delegate application] == NO )
        {
            NSLog( @"🔴 [ERROR] Invalid initialize application delegate: %@"
                , className
            );

            return EXIT_FAILURE;
        }
    }

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

    NSLog(@"🟢 Mengine application finish");

    return EXIT_SUCCESS;
}
