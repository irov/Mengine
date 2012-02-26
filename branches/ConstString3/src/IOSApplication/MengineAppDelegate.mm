//
//  MengineAppDelegate.m
//  Mengine
//
//  Created by Dgut on 9/14/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "MengineAppDelegate.h"
#import "EAGLView.h"
#import "Menge/Application.h"

Menge::iOSApplication * pApplication = 0;

@implementation MengineAppDelegate

@synthesize window;
@synthesize glView;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions   
{
	[ UIApplication sharedApplication ].statusBarOrientation = UIInterfaceOrientationLandscapeRight;
	
	pApplication = new Menge::iOSApplication();
	
	if( !pApplication->Init() )
		return NO;
	
    [glView startAnimation];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    NSLog( @"applicationWillResignActive" );
    [glView stopAnimation];
    
    if( pApplication != 0 )
	{
        Menge::Application * app = pApplication->getApplication();
        app->onFocus( false );   
    }
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    NSLog( @"applicationDidBecomeActive" );
    
    if( pApplication != 0 )
	{
        Menge::Application * app = pApplication->getApplication();
        app->onFocus( true );
    }
    
    [glView startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    NSLog( @"applicationWillTerminate" );
    
    [glView stopAnimation];
    if( pApplication != 0 )
	{
        Menge::Application * app = pApplication->getApplication();
        app->onFocus( false );
    }
}

- (void)dealloc
{
    //NSLog( @"pApplication dealloc" );
    
    delete pApplication;
    pApplication = 0;
    
    [window release];
    window = nil;
    
    [glView release];
    glView = nil;    

    [super dealloc];
}

@end
