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

void InterruptionListenerCallback(void *data, UInt32 interruptionState)
{
    if (interruptionState == kAudioSessionBeginInterruption)
    {
        NSLog( @"InterruptionListenerCallback kAudioSessionBeginInterruption" );
        
        AudioSessionSetActive(false);
        
        pApplication->AudioSessionBeginInterruption();
        
        //alcMakeContextCurrent(NULL);
        //alcSuspendContext(soundContext);
    }
    else if (interruptionState == kAudioSessionEndInterruption)
    {
        NSLog( @"InterruptionListenerCallback kAudioSessionEndInterruption" );
        
        UInt32 category = kAudioSessionCategory_AmbientSound;
        AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
        
        OSStatus error = AudioSessionSetActive(true);
        
        int restartTry = 0;
        
        while (error != 0 && ++restartTry < 10)
        {
            sleep(1);
            error = AudioSessionSetActive(true);
        }
        
        if (error == 0)
        {
            pApplication->AudioSessionEndInterruption();
            //alcMakeContextCurrent(soundContext);
            //alcProcessContext(soundContext);
        } 
    }
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions   
{
	[ UIApplication sharedApplication ].statusBarOrientation = UIInterfaceOrientationLandscapeRight;
    
    AudioSessionInitialize(NULL, NULL, InterruptionListenerCallback, NULL);
	
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
    
//    if (CheckForActiveBackgroundAudioSession()) 
//    { 
//        TurnSoundOff();
//    } 
    
    [self performSelector:@selector(SoundResumeOnHotSwapAntiBug) withObject:nil afterDelay:1.5]; 
}

- (void)SoundResumeOnHotSwapAntiBug
{
    NSLog( @"SoundResumeOnHotSwapAntiBug" );
    
//    if (!CheckForActiveBackgroundAudioSession())
//    {
//        TurnSoundOn();
//    } 
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
