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

Menge::iOSApplication * pApplication;

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
    [glView stopAnimation];
	Menge::Application::get()->onFocus( false );
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
	Menge::Application::get()->onFocus( true );
    [glView startAnimation];
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    [glView stopAnimation];
	Menge::Application::get()->onFocus( false );
}

- (void)dealloc
{
    [window release];
    [glView release];

    [super dealloc];
}

@end
