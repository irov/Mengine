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
    
    if( pApplication == 0 )
	{
        return;
    }
    
    Menge::ApplicationInterface * app = pApplication->getApplication();
    app->onFocus( false );
    
    NSLog( @"applicationWillResignActive TurnSoundOff" );
    AudioSessionSetActive(NO);
    
    pApplication->TurnSoundOff();
    
    [glView stopAnimation];
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    NSLog( @"applicationDidBecomeActive" );
    
    if( pApplication == 0 )
	{
        return;
    }
     
    Menge::ApplicationInterface * app = pApplication->getApplication();
    app->onFocus( true );
    
    [glView startAnimation];

    
    [self performSelector:@selector(SoundResumeOnHotSwapAntiBug) withObject:nil afterDelay:1.5]; 
}

- (void)SoundResumeOnHotSwapAntiBug
{
    NSLog( @"SoundResumeOnHotSwapAntiBug" );
    
    if( pApplication == 0 )
	{
        return;
    }
    
    //if( pApplication->OpenAL_OtherAudioIsPlaying() == false ) 
    {
        NSLog( @"applicationDidBecomeActive TurnSoundOn" );
        
        UInt32 category = kAudioSessionCategory_AmbientSound;
        AudioSessionSetProperty ( kAudioSessionProperty_AudioCategory, sizeof (category), &category );
        
        AudioSessionSetActive(YES);
        
        pApplication->TurnSoundOn();
    } 
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    NSLog( @"applicationWillTerminate" );
    
    if( pApplication == 0 )
	{
        return;
    }
    
    [glView stopAnimation];

    Menge::ApplicationInterface * app = pApplication->getApplication();
    app->onFocus( false );
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
