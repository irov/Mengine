#include "AppleOpenUrlInDefaultBrowser.h"

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

int AppleOpenUrlInDefaultBrowser( const char * _url )
{
    NSString * url = [NSString stringWithUTF8String: _url];
    
    [[UIApplication sharedApplication] openURL: [NSURL URLWithString:url]];

    return 0;
}
