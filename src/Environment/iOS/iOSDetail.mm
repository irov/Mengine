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

            NSString * PluginBundle_Value = [MengineAppleAdjustPluginBundle objectForKey:_key];

            if (PluginBundle_Value == nil) {
                return _default;
            }

            return PluginBundle_Value;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
