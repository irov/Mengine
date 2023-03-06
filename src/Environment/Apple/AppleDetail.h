#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

namespace Mengine
{
    namespace Helper
    {
        NSString * AppleGetBundleIdentifier();
    
        BOOL AppleHasBundlePluginConfig( NSString * _plugin );
        NSDictionary * AppleGetBundlePluginConfig( NSString * _plugin );
        NSString * AppleGetBundlePluginConfigString( NSString * _plugin, NSString * _key, NSString * _default );
        NSNumber * AppleGetBundlePluginConfigNumber( NSString * _plugin, NSString * _key, NSNumber * _default );
        NSInteger AppleGetBundlePluginConfigInteger( NSString * _plugin, NSString * _key, NSInteger _default );
        double AppleGetBundlePluginConfigDouble( NSString * _plugin, NSString * _key, double _default );
        BOOL AppleGetBundlePluginConfigBoolean( NSString * _plugin, NSString * _key, BOOL _default );
    }
}
