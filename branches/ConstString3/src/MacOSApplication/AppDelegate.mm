//
//  AppDelegate.m
//  Mengine
//
//  Created by Dgut on 26.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import "AppDelegate.h"
#import "Application.h"

Menge::MacOSXApplication * pApplication;

bool needFullscreen = false;
bool fullscreen = false;

@implementation AppDelegate

@synthesize window;// = _window;
@synthesize view;

- (void)dealloc
{
    [super dealloc];
    [timer invalidate];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
    
    window.delegate = ( id< NSWindowDelegate > )window;
    window.acceptsMouseMovedEvents = YES;
    
    pApplication = new Menge::MacOSXApplication();
	
	if( !pApplication->Init() )
        pApplication->stop();
    
    [ window makeKeyAndOrderFront : self ];
    
    timer = [ [ NSTimer scheduledTimerWithTimeInterval:1./FPS
                                                target:self
                                              selector:@selector( tick: )
                                              userInfo:nil repeats:YES ] retain ];
    
    [ [ NSRunLoop currentRunLoop ] addTimer:timer
                                    forMode:NSEventTrackingRunLoopMode ];
    [ [ NSRunLoop currentRunLoop ] addTimer:timer
                                    forMode:NSModalPanelRunLoopMode ];
}

- (void) tick:(NSTimer *)timer
{
    [ window display ];
    if( needFullscreen != fullscreen )
    {
        fullscreen = needFullscreen;
        [ self setFullscreen : fullscreen ];
        [ window makeFirstResponder : view ];
         //Menge::Application::get()->onFocus( true );
    }
}

-( void ) setFullscreen : ( BOOL )fullscreen
{
    static int count = 0;
    
    NSLog( @"fullscreen=%i", ++count );
    
    if( fullscreen )
    {
        window.styleMask = NSBorderlessWindowMask;
        window.level = NSMainMenuWindowLevel + 1;
        
        //[ window setOpaque : YES ];
        window.hidesOnDeactivate = YES;
        
        NSRect mainDisplayRect = [ [ NSScreen mainScreen ] frame ];
        
        [ window setFrame : mainDisplayRect display : YES ];
        
        [ window makeKeyAndOrderFront : self ];
    }
    else
    {
        window.styleMask = NSTitledWindowMask | NSMiniaturizableWindowMask | NSClosableWindowMask;// | NSResizableWindowMask;
        window.level = NSNormalWindowLevel;
        
        //[ window setOpaque : NO ];
        window.hidesOnDeactivate = NO;
        
        NSRect mainDisplayRect = [ [ NSScreen mainScreen ] frame ];
        mainDisplayRect.origin.x = mainDisplayRect.size.width / 2 - 1024 / 2;
        mainDisplayRect.origin.y = mainDisplayRect.size.height / 2 - 768 / 2;
        
        mainDisplayRect.size.width = 1024;
        mainDisplayRect.size.height = 768;
        
        NSRect content = [ NSWindow contentRectForFrameRect : mainDisplayRect styleMask: NSTitledWindowMask | NSMiniaturizableWindowMask | NSClosableWindowMask ];
        
        mainDisplayRect.size.height = mainDisplayRect.size.height * 2 - content.size.height;
        
        [ window setFrame : mainDisplayRect display : YES ];
        
        [ window makeKeyAndOrderFront : self ];
    }
}

- (void)applicationDidBecomeActive:(NSNotification *)aNotification
{
    Menge::Application::get()->onFocus( true );
    //pApplication->FocusCursor( true );
    NSLog( @"Focus=true" );
}

- (void)applicationDidResignActive:(NSNotification *)aNotification
{
    Menge::Application::get()->onFocus( false );
    //pApplication->FocusCursor( false );
    NSLog( @"Focus=false" );
}

- (IBAction)miniaturize:(id)sender
{
    Menge::Application::get()->onFocus( false );
    //pApplication->FocusCursor( false );
    NSLog( @"Focus=false" );
}

- (IBAction)deminiaturize:(id)sender
{
    Menge::Application::get()->onFocus( true );
    //pApplication->FocusCursor( true );
    NSLog( @"Focus=true" );
}

@end
