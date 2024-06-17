#pragma once

#include "Config/Config.h"

#import "Environment/Apple/AppleEvent.h"

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
        void iOSPluginApplicationDelegateEventNotify( MengineEvent * name, id firstArg, ... );
        NSString * iOSPathForTemporaryFileWithPrefix( NSString * prefix, NSString * ext );
        void iOSAlert( UIViewController * view, NSString * title, NSString * message );
    }
}
