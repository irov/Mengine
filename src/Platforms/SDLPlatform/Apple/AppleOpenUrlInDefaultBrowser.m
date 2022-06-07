#include "AppleOpenUrlInDefaultBrowser.h"

#import <Foundation/Foundation.h>

#if defined(MENGINE_PLATFORM_OSX)
#import <AppKit/AppKit.h>
#else
#import <UIKit/UIKit.h>
#endif

int AppleOpenUrlInDefaultBrowser( const char * _url )
{
    NSString * url = [NSString stringWithUTF8String: _url];
    
#if defined(MENGINE_PLATFORM_OSX)
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:url]];
#else
    if( [[UIApplication sharedApplication] canOpenURL: [NSURL URLWithString:url]] == TRUE )
    {
        [[UIApplication sharedApplication] openURL: [NSURL URLWithString:url] options:@{} completionHandler:^(BOOL success)
        {
            //ToDo callback
        }];
    }
    else
    {
        return -1;
    }
#endif
    return 0;
}
