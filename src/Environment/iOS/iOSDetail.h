#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

namespace Mengine
{
    namespace Helper
    {
        UIViewController * iOSGetRootViewController();
        NSUUID * iOSGetAdIdentifier();
        BOOL iOSHasBundlePluginConfig( NSString * _plugin );
        NSDictionary * iOSGetBundlePluginConfig( NSString * _plugin );
        NSString * iOSGetBundlePluginConfigString( NSString * _plugin, NSString * _key, NSString * _default );
        NSNumber * iOSGetBundlePluginConfigNumber( NSString * _plugin, NSString * _key, NSNumber * _default );
        NSInteger iOSGetBundlePluginConfigInteger( NSString * _plugin, NSString * _key, NSInteger _default );
        double iOSGetBundlePluginConfigDouble( NSString * _plugin, NSString * _key, double _default );
        BOOL iOSGetBundlePluginConfigBoolean( NSString * _plugin, NSString * _key, BOOL _default );
    }
}
