#pragma once

#include "Config/Config.h"

#import "Environment/iOS/UIPluginApplicationDelegateInterface.h"

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

namespace Mengine
{
    namespace Helper
    {
        UIViewController * iOSGetRootViewController();
        NSUUID * iOSGetAdIdentifier();
        id iOSGetUIProxyApplicationDelegate( Class delegateClass );
        void iOSPluginApplicationDelegateEventNotify( NSString * name, id firstArg, ... );
    }
}
