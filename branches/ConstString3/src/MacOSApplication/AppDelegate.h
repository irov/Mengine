//
//  AppDelegate.h
//  Mengine
//
//  Created by Dgut on 26.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "MacOSXApplication.h"

#define FPS 60

extern Menge::MacOSXApplication * pApplication;

@interface AppDelegate : NSObject <NSApplicationDelegate>
{
    NSTimer * timer;
    NSWindow * window;
    NSView * view;
}

@property (assign) IBOutlet NSWindow * window;
@property (assign) IBOutlet NSView * view;

@end

extern bool needFullscreen;