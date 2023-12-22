#pragma once

#include "Config/Config.h"

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
        void iOSPluginApplicationDelegateEventNotify( NSString * name, id firstArg, ... );
        NSString * iOSPathForTemporaryFileWithPrefix( NSString * prefix, NSString * ext );
    }
}
