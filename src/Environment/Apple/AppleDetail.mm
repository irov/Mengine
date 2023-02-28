#include "AppleDetail.h"

#import <AdSupport/ASIdentifierManager.h>

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        NSString * AppleGetBundleIdentifier()
        {
            NSBundle * mainBundle = [NSBundle mainBundle];
            
            NSString * bundleIdentifier = [mainBundle bundleIdentifier];
            
            return bundleIdentifier;
        }
        //////////////////////////////////////////////////////////////////////////
        BOOL AppleHasBundlePluginConfig( NSString * _plugin )
        {
            NSBundle * mainBundle = [NSBundle mainBundle];
            
            if ([mainBundle objectForInfoDictionaryKey:_plugin] == nil) {
                return NO;
            }

            return YES;
        }
        //////////////////////////////////////////////////////////////////////////
        NSDictionary * AppleGetBundlePluginConfig( NSString * _plugin )
        {
            NSBundle * mainBundle = [NSBundle mainBundle];
            
            NSDictionary * PluginBundle = [mainBundle objectForInfoDictionaryKey:_plugin];

            if (PluginBundle == nil) {
                return nil;
            }

            return PluginBundle;
        }
        //////////////////////////////////////////////////////////////////////////
        NSString * AppleGetBundlePluginConfigString( NSString * _plugin, NSString * _key, NSString * _default )
        {
            NSDictionary * PluginBundle = Helper::AppleGetBundlePluginConfig( _plugin );

            if (PluginBundle == nil) {
                return _default;
            }

            NSString * PluginBundle_Value = [PluginBundle objectForKey:_key];

            if (PluginBundle_Value == nil) {
                return _default;
            }

            return PluginBundle_Value;
        }
        //////////////////////////////////////////////////////////////////////////
        NSNumber * AppleGetBundlePluginConfigNumber( NSString * _plugin, NSString * _key, NSNumber * _default )
        {
            NSDictionary * PluginBundle = Helper::AppleGetBundlePluginConfig( _plugin );

            if (PluginBundle == nil) {
                return _default;
            }

            NSNumber * PluginBundle_Number = [PluginBundle objectForKey:_key];

            if (PluginBundle_Number == nil) {
                return _default;
            }

            return PluginBundle_Number;
        }
        //////////////////////////////////////////////////////////////////////////
        NSInteger AppleGetBundlePluginConfigInteger( NSString * _plugin, NSString * _key, NSInteger _default )
        {
            NSDictionary * PluginBundle = Helper::AppleGetBundlePluginConfig( _plugin );

            if (PluginBundle == nil) {
                return _default;
            }

            NSNumber * PluginBundle_Number = [PluginBundle objectForKey:_key];

            if (PluginBundle_Number == nil) {
                return _default;
            }

            NSInteger PluginBundle_Value = [PluginBundle_Number integerValue];
            
            return PluginBundle_Value;
        }
        //////////////////////////////////////////////////////////////////////////
        double AppleGetBundlePluginConfigDouble( NSString * _plugin, NSString * _key, double _default )
        {
            NSDictionary * PluginBundle = Helper::AppleGetBundlePluginConfig( _plugin );

            if (PluginBundle == nil) {
                return _default;
            }

            NSNumber * PluginBundle_Number = [PluginBundle objectForKey:_key];

            if (PluginBundle_Number == nil) {
                return _default;
            }

            double PluginBundle_Value = [PluginBundle_Number doubleValue];
            
            return PluginBundle_Value;
        }
        //////////////////////////////////////////////////////////////////////////
        BOOL AppleGetBundlePluginConfigBoolean( NSString * _plugin, NSString * _key, BOOL _default )
        {
            NSDictionary * PluginBundle = Helper::AppleGetBundlePluginConfig( _plugin );

            if (PluginBundle == nil) {
                return _default;
            }

            NSNumber * PluginBundle_Number = [PluginBundle objectForKey:_key];

            if (PluginBundle_Number == nil) {
                return _default;
            }

            BOOL PluginBundle_Value = [PluginBundle_Number boolValue];
            
            return PluginBundle_Value;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
