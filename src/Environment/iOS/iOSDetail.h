#pragma once

#include "Config/Config.h"

#import "Environment/Apple/AppleEvent.h"
#import "Environment/iOS/iOSAdRevenueParam.h"

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

namespace Mengine
{
    namespace Helper
    {
        UIWindow * iOSGetRootWindow();
        UIView * iOSGetRootView();
        UIViewController * iOSGetRootViewController();
        NSUUID * iOSGetAdIdentifier();
        id iOSGetUIProxyApplicationDelegate( Class delegateClass );
        void iOSEventNotify( AppleEvent * event, id firstArg, ... );
        void iOSEventNotifyArray( AppleEvent * event, NSArray<id> * args );
        void iOSAdRevenue( iOSAdRevenueParam * revenue );
        NSString * iOSPathForTemporaryFileWithPrefix( NSString * prefix, NSString * ext );
        void iOSAlert( UIViewController * view, NSString * title, NSString * message );
    }
}
