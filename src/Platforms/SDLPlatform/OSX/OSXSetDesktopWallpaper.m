#include "OSXSetDesktopWallpaper.h"

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

int OSXSetDesktopWallpaper( const char * _url )
{ 
	NSURL *url = [NSURL fileURLWithPath:@(_url)];

    NSScreen* currentScreen = [NSScreen mainScreen];
    if (!currentScreen) {
      return -1;
    }
    	
    NSDictionary* screenOptions =
        [[NSWorkspace sharedWorkspace] desktopImageOptionsForScreen:currentScreen];
    
    NSError *error = nil;
    
    if (![[NSWorkspace sharedWorkspace] setDesktopImageURL:url
                                                   forScreen:currentScreen
                                                     options:screenOptions
                                                       error:&error]) {
        return (int)[error code];
    }

    return 0;
}
