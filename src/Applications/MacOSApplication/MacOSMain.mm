#import "Environment/MacOS/MacOSProxyApplicationDelegateInterface.h"
#import "Environment/MacOS/MacOSApplication.h"
#import "Environment/MacOS/MacOSUtils.h"

#import "MacOSApplicationDelegates.h"
#import "MacOSApplicationDelegate.h"

#import "MacOSApplication.h"

#import "Kernel/Configuration.h"

#import "Config/StdString.h"

#import <AppKit/AppKit.h>

//////////////////////////////////////////////////////////////////////////
static bool hasCLILaunchArgument( int _argc, char * _argv[] )
{
    for( int index = 1; index != _argc; ++index )
    {
        if( Mengine::StdString::strcmp( _argv[index], "--cli" ) == 0 )
        {
            return true;
        }
    }

    return false;
}
//////////////////////////////////////////////////////////////////////////
int main( int argc, char * argv[] )
{
    @autoreleasepool
    {
        Mengine::Configuration configuration;
        bool CLI = hasCLILaunchArgument( argc, argv );
        configuration.silentDialog = CLI;
        configuration.muteSound = CLI;

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
                if( configuration.silentDialog == false )
                {
                    Mengine::Helper::MacOSShowFatalAlert( "Mengine initialize", [message UTF8String] );
                }

                return EXIT_FAILURE;
            }

            id<MacOSProxyApplicationDelegateInterface> delegate = [clazz alloc];

            if( [delegate application] == NO )
            {
                NSLog( @"[ERROR] Invalid initialize application delegate: %@"
                    , className
                );

                NSString * message = [NSString stringWithFormat:@"Invalid initialize application delegate: %@", className];
                if( configuration.silentDialog == false )
                {
                    Mengine::Helper::MacOSShowFatalAlert( "Mengine initialize", [message UTF8String] );
                }

                return EXIT_FAILURE;
            }
        }

        Mengine::MacOSApplication application;

        if( application.bootstrap( argc, argv, configuration ) == false )
        {
            if( configuration.silentDialog == false )
            {
                Mengine::Helper::MacOSShowFatalAlert( "Mengine initialize", "Mengine invalid bootstrap" );
            }

            application.finalize();

            return EXIT_FAILURE;
        }

        if( application.initialize() == false )
        {
            if( configuration.silentDialog == false )
            {
                Mengine::Helper::MacOSShowFatalAlert( "Mengine initialize", "Mengine invalid initialize" );
            }

            application.finalize();

            return EXIT_FAILURE;
        }

        application.loop();
        application.finalize();

        return EXIT_SUCCESS;
    }
}
