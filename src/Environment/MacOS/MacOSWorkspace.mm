#import "MacOSWorkspace.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleLog.h"

#import "Kernel/Logger.h"

#import <AppKit/AppKit.h>

@implementation MacOSWorkspace

- (instancetype)init {
    self = [super init];

    [[[NSWorkspace sharedWorkspace] notificationCenter] addObserver:self
                                                           selector:@selector(workspaceActiveSpaceDidChangeNotification:)
                                                               name:NSWorkspaceActiveSpaceDidChangeNotification object:nil];

    return self;
}

- (void)dealloc {
    [[[NSWorkspace sharedWorkspace] notificationCenter] removeObserver:self];
}

- (void)changeDesktopWallpaper:(const char *) _path {
    NSString* urlString = [NSString stringWithUTF8String:_path];

    if( urlString == nil )
    {
        return;
    }

    NSURL * url = [NSURL fileURLWithPath:urlString];

    m_urlWallpaper = url;
}

- (void)workspaceActiveSpaceDidChangeNotification:(NSNotification*) notification {
    LOGGER_INFO("platform", "workspaceActiveSpaceDidChangeNotification");

    if( m_urlWallpaper != nil )
    {
        NSURL * url = m_urlWallpaper;
        m_urlWallpaper = nil;

        NSScreen * mainScreen = [NSScreen mainScreen];

        if( mainScreen == nil )
        {
            return;
        }

        NSDictionary * screenOptions = [[NSWorkspace sharedWorkspace] desktopImageOptionsForScreen:mainScreen];

        if( screenOptions == nil )
        {
            return;
        }

        NSError *error = nil;
        if( [[NSWorkspace sharedWorkspace] setDesktopImageURL:url
                                                    forScreen:mainScreen
                                                      options:screenOptions
                                                        error:&error] == FALSE )
        {
            [AppleLog withFormat:@"invalid set desktop image url '%@' error '%@'"
                , url.absoluteString
                , [AppleDetail getMessageFromNSError:error]
            ];

            return;
        }
    }
}

@end
