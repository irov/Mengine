#import "Environment/MacOS/MacOSProxyApplicationDelegateInterface.h"
#import "Environment/MacOS/MacOSApplication.h"
#import "Environment/MacOS/MacOSUtils.h"

#import "MacOSApplicationDelegates.h"
#import "MacOSApplicationDelegate.h"

#import "MacOSApplication.h"

#import <AppKit/AppKit.h>

int main( int argc, char * argv[] )
{
    @autoreleasepool
    {
        [NSApplication sharedApplication];
        MacOSApplicationDelegate * applicationDelegate = [[MacOSApplicationDelegate alloc] init];
        [NSApp setDelegate:applicationDelegate];

        [MacOSApplication.sharedInstance initialize];

        NSArray * proxysClassed = [MacOSApplicationDelegates getApplicationDelegates];

        for( NSString * className in proxysClassed )
        {
            Class clazz = NSClassFromString( className );

            if( clazz == nil )
            {
                NSLog( @"[ERROR] Invalid found application delegate: %@"
                    , className
                );

                NSString * message = [NSString stringWithFormat:@"Invalid found application delegate: %@", className];
                Mengine::Helper::MacOSShowFatalAlert( "Mengine initialize", [message UTF8String] );

                return EXIT_FAILURE;
            }

            id<MacOSProxyApplicationDelegateInterface> delegate = [clazz alloc];

            if( [delegate application] == NO )
            {
                NSLog( @"[ERROR] Invalid initialize application delegate: %@"
                    , className
                );

                NSString * message = [NSString stringWithFormat:@"Invalid initialize application delegate: %@", className];
                Mengine::Helper::MacOSShowFatalAlert( "Mengine initialize", [message UTF8String] );

                return EXIT_FAILURE;
            }
        }

        Mengine::MacOSApplication application;

        if( application.bootstrap( argc, argv ) == false )
        {
            Mengine::Helper::MacOSShowFatalAlert( "Mengine initialize", "Mengine invalid bootstrap" );

            application.finalize();

            return EXIT_FAILURE;
        }

        if( application.initialize() == false )
        {
            Mengine::Helper::MacOSShowFatalAlert( "Mengine initialize", "Mengine invalid initialize" );

            application.finalize();

            return EXIT_FAILURE;
        }

        application.loop();
        application.finalize();

        return EXIT_SUCCESS;
    }
}
