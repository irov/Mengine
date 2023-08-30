#include "iOSDetail.h"

#import "UIMainApplicationDelegateInterface.h"
#import "UIPluginApplicationDelegateInterface.h"

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
        id iOSGetUIProxyApplicationDelegate( Class delegateClass )
        {
            NSObject<UIMainApplicationDelegateInterface> * delegate = (NSObject<UIMainApplicationDelegateInterface> *)[[UIApplication sharedApplication] delegate];

            NSArray<UIPluginApplicationDelegateInterface> * pluginDelegates = [delegate getPluginDelegates];

            for( NSObject<UIPluginApplicationDelegateInterface> * delegate : pluginDelegates ) {
                if( [delegate isMemberOfClass:delegateClass] == YES ) {
                    return delegate;
                }
            }

            return nil;
        }
        //////////////////////////////////////////////////////////////////////////
        void iOSPluginApplicationDelegateEventNotify( NSString * name, id firstArg, ... )
        {
            NSObject<UIMainApplicationDelegateInterface> * delegate = (NSObject<UIMainApplicationDelegateInterface> *)[[UIApplication sharedApplication] delegate];
            
            va_list args;
            va_start(args, firstArg);
            
            NSMutableArray * send_args = [[NSMutableArray alloc] init];

            for( NSString *arg = firstArg; arg != nil; arg = va_arg(args, NSString*) ) {
                [send_args addObject:firstArg];
            }

            va_end(args);
            
            [delegate notify:name arrayArgs:send_args];
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
