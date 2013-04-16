//
//  MengineWindow.m
//  Mengine
//
//  Created by Dgut on 26.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import "MengineWindow.h"
#import "AppDelegate.h"
#import "Menge/Application.h"

@implementation MengineWindow

/*- (id)init
{
    self = [super init];
    if (self) {
        self.delegate = self;
        // Initialization code here.
    }
    
    return self;
}*/

- (BOOL)windowShouldClose:(id)sender
{    
    [ [ NSApplication sharedApplication ] terminate : self ];
    
    return YES;
}

- (void)windowWillMiniaturize:(NSNotification *)notification
{
    Menge::Application::get()->onFocus( false );
    //pApplication->FocusCursor( false );
    NSLog( @"Focus=false" );
}

- (void)windowDidDeminiaturize:(NSNotification *)notification
{
    Menge::Application::get()->onFocus( true );
    //pApplication->FocusCursor( true );
    NSLog( @"Focus=true" );
}

@end
