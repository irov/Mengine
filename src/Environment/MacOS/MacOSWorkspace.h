#pragma once

#import <Foundation/Foundation.h>

@interface MacOSWorkspace : NSObject {
    NSURL * m_urlWallpaper;
}

- (instancetype)init;
- (void)dealloc;

- (void)changeDesktopWallpaper:(const char *) _path;
- (void)workspaceActiveSpaceDidChangeNotification:(NSNotification*) notification;

@end
