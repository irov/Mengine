#include "OSXSetDesktopWallpaper.h"

#ifdef __APPLE__

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

void OSXSetDesktopWallpaper( const char * _url )
{ 
	NSURL *URL = [NSURL fileURLWithPath:@(_url)];
	NSDictionary<NSWorkspaceDesktopImageOptionKey, id> *options = @{NSWorkspaceDesktopImageScalingKey : @3, NSWorkspaceDesktopImageAllowClippingKey : @0, NSWorkspaceDesktopImageFillColorKey : [NSColor colorWithSRGBRed:red green:green blue:blue alpha:1.0]};
	NSError *error = nil;

	BOOL success = [sharedworkspace setDesktopImageURL:[URL absoluteURL] forScreen:screen options:options error:&error];
	if (!success) {
		NSLog(@"ERROR DURING DESKTOP BACKGROUND CHANGE: %@", error);
	}
}

#endif /* __APPLE__ */

/* vi: set ts=4 sw=4 expandtab: */
