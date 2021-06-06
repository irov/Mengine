#include "OSXOpenUrlInDefaultBrowser.h"

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

int OSXOpenUrlInDefaultBrowser( const char * _url )
{
    NSURL* url = [[NSURL alloc] initWithString: @(_url)];
    if (![[NSWorkspace sharedWorkspace] openURL: url])
    {
        return -1;
    }

    return 0;
}
