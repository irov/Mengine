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

#include <AudioToolbox/AudioToolbox.h>

Menge::iOSApplication * pApplication = 0;

@implementation MengineAppDelegate

@synthesize window;
@synthesize glView;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions   
{
	[ UIApplication sharedApplication ].statusBarOrientation = UIInterfaceOrientationLandscapeRight;
    
    NSLog( @"AudioSessionInitialize InterruptionListenerCallback" );
    
	
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

static bool OpenAL_OtherAudioIsPlaying()
{
     int playing;
     UInt32 size = sizeof(playing);
     AudioSessionGetProperty(kAudioSessionProperty_OtherAudioIsPlaying, &size, &playing);
     return (bool)playing;
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
    
    if (OpenAL_OtherAudioIsPlaying()) 
    { 
        app->TurnSoundOff();
    } 
    
    [self performSelector:@selector(SoundResumeOnHotSwapAntiBug) withObject:nil afterDelay:1.5]; 
}

- (void)SoundResumeOnHotSwapAntiBug
{
    NSLog( @"SoundResumeOnHotSwapAntiBug" );
    
    if (!OpenAL_OtherAudioIsPlaying())
    {
        app->TurnSoundOn();
    } 
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
    NSLog( @"pApplication dealloc" );
    
    delete pApplication;
    pApplication = 0;
    
    [window release];
    window = nil;
    
    [glView release];
    glView = nil;    

    [super dealloc];
}

@end
