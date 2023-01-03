#include "iOSDetail.h"

#import <AdSupport/ASIdentifierManager.h>

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        UIViewController * iOSGetRootViewController()
        {
            UIWindow * window = [UIApplication sharedApplication].delegate.window;

            UIViewController * viewController = window.rootViewController;
            
            return viewController;
        }
        //////////////////////////////////////////////////////////////////////////
        NSUUID * iOSGetAdIdentifier()
        {
            NSUUID * idfa_uuid = [[ASIdentifierManager sharedManager] advertisingIdentifier];
            
            return idfa_uuid;
        }
        //////////////////////////////////////////////////////////////////////////
        BOOL iOSHasBundlePluginConfig( NSString * _plugin )
        {
            NSBundle * mainBundle = [NSBundle mainBundle];
            
            if ([mainBundle objectForInfoDictionaryKey:_plugin] == nil) {
                return NO;
            }

            return YES;
        }
        //////////////////////////////////////////////////////////////////////////
        NSString * iOSGetBundlePluginConfigString( NSString * _plugin, NSString * _key, NSString * _default )
        {
            NSBundle * mainBundle = [NSBundle mainBundle];
            
            NSDictionary<NSString *, id> * PluginBundle = [mainBundle objectForInfoDictionaryKey:_plugin];

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
        NSNumber * iOSGetBundlePluginConfigNumber( NSString * _plugin, NSString * _key, NSNumber * _default )
        {
            NSBundle * mainBundle = [NSBundle mainBundle];
            
            NSDictionary<NSString *, id> * PluginBundle = [mainBundle objectForInfoDictionaryKey:_plugin];

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
        NSInteger iOSGetBundlePluginConfigInteger( NSString * _plugin, NSString * _key, NSInteger _default )
        {
            NSBundle * mainBundle = [NSBundle mainBundle];
            
            NSDictionary<NSString *, id> * PluginBundle = [mainBundle objectForInfoDictionaryKey:_plugin];

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
        double iOSGetBundlePluginConfigDouble( NSString * _plugin, NSString * _key, double _default )
        {
            NSBundle * mainBundle = [NSBundle mainBundle];
            
            NSDictionary<NSString *, id> * PluginBundle = [mainBundle objectForInfoDictionaryKey:_plugin];

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
        BOOL iOSGetBundlePluginConfigBoolean( NSString * _plugin, NSString * _key, BOOL _default )
        {
            NSBundle * mainBundle = [NSBundle mainBundle];
            
            NSDictionary<NSString *, id> * PluginBundle = [mainBundle objectForInfoDictionaryKey:_plugin];

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
