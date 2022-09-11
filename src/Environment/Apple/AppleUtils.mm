#include "AppleUtils.h"

#if defined(MENGINE_PLATFORM_MACOS)
#   import <AppKit/AppKit.h>
#else
#   import <UIKit/UIKit.h>
#endif

namespace Mengine
{
    namespace Helper
    {
        /////////////////////////////////////////////////////////////////////////
        bool AppleOpenUrlInDefaultBrowser( const Char * _url )
        {
            NSString * url = [NSString stringWithUTF8String: _url];
    
        #if defined(MENGINE_PLATFORM_MACOS)
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
                return false;
            }
        #endif
            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
