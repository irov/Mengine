#include "AppleOpenUrlInDefaultBrowser.h"

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

int AppleOpenUrlInDefaultBrowser( const char * _url )
{
    NSString * url = [NSString stringWithUTF8String: _url];
    
    if( [[UIApplication sharedApplication] canOpenURL: [NSURL URLWithString:url]] == TRUE )
    {
        [[UIApplication sharedApplication] openURL: [NSURL URLWithString:url] options:@{} completionHandler:nil];
    }
    else
    {
        return -1;
    }

    return 0;
}
