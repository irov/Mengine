#pragma once

#import <Foundation/Foundation.h>

@interface SDLPlatformMacOSWorkspace : NSObject {
    NSURL * m_urlWallpaper;
}

- (BOOL)initialize;
- (void)finalize;
- (void)changeDesktopWallpaper:(const char *) _path;


- (void)workspaceActiveSpaceDidChangeNotification:(NSNotification*) notification;

@end
