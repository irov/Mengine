#include "iOSDetail.h"

#import "iOSUIMainApplicationDelegateInterface.h"
#import "iOSPluginApplicationDelegateInterface.h"

#import <AdSupport/ASIdentifierManager.h>

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        UIWindow * iOSGetRootWindow()
        {
            UIApplication * application = [UIApplication sharedApplication];
            id<UIApplicationDelegate> delegate = application.delegate;
            UIWindow * window = delegate.window;
            
            return window;
        }
        //////////////////////////////////////////////////////////////////////////
        UIView * iOSGetRootView()
        {
            UIWindow * window = Helper::iOSGetRootWindow();
            
            UIView * view = [window.subviews objectAtIndex:0];
            
            return view;
        }
        //////////////////////////////////////////////////////////////////////////
        UIViewController * iOSGetRootViewController()
        {
            UIWindow * window = Helper::iOSGetRootWindow();

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
            NSObject<iOSUIMainApplicationDelegateInterface> * delegate = (NSObject<iOSUIMainApplicationDelegateInterface> *)[[UIApplication sharedApplication] delegate];

            NSArray<iOSPluginApplicationDelegateInterface> * pluginDelegates = [delegate getPluginApplicationDelegates];

            for( NSObject<iOSPluginApplicationDelegateInterface> * delegate : pluginDelegates ) {
                if( [delegate isMemberOfClass:delegateClass] == YES ) {
                    return delegate;
                }
            }

            return nil;
        }
        //////////////////////////////////////////////////////////////////////////
        void iOSEventNotify( AppleEvent * event, id firstArg, ... )
        {
            NSObject<iOSUIMainApplicationDelegateInterface> * delegate = (NSObject<iOSUIMainApplicationDelegateInterface> *)[[UIApplication sharedApplication] delegate];
            
            va_list args;
            va_start(args, firstArg);
            
            NSMutableArray * send_args = [[NSMutableArray alloc] init];

            for( NSString *arg = firstArg; arg != nil; arg = va_arg(args, NSString*) ) {
                [send_args addObject:firstArg];
            }

            va_end(args);
            
            [delegate notify:event arrayArgs:send_args];
        }
        //////////////////////////////////////////////////////////////////////////
        void iOSEventNotifyArray( AppleEvent * event, NSArray<id> * args )
        {
            NSObject<iOSUIMainApplicationDelegateInterface> * delegate = (NSObject<iOSUIMainApplicationDelegateInterface> *)[[UIApplication sharedApplication] delegate];
            
            [delegate notify:event arrayArgs:args];
        }
        //////////////////////////////////////////////////////////////////////////
        void iOSAdRevenue( iOSAdRevenueParam * revenue )
        {
            NSObject<iOSUIMainApplicationDelegateInterface> * delegate = (NSObject<iOSUIMainApplicationDelegateInterface> *)[[UIApplication sharedApplication] delegate];

            [delegate eventAdRevenue:revenue];
        }
        //////////////////////////////////////////////////////////////////////////
        void iOSTransparencyConsent( iOSTransparencyConsentParam * consent )
        {
            NSObject<iOSUIMainApplicationDelegateInterface> * delegate = (NSObject<iOSUIMainApplicationDelegateInterface> *)[[UIApplication sharedApplication] delegate];

            [delegate eventTransparencyConsent:consent];
        }
        //////////////////////////////////////////////////////////////////////////
        NSString * iOSPathForTemporaryFileWithPrefix( NSString * prefix, NSString * ext )
        {
            NSString * result;
            CFUUIDRef uuid;
            CFStringRef uuidStr;

            uuid = CFUUIDCreate(NULL);
            assert(uuid != NULL);

            uuidStr = CFUUIDCreateString(NULL, uuid);
            assert(uuidStr != NULL);

            result = [NSTemporaryDirectory() stringByAppendingPathComponent:[NSString stringWithFormat:@"%@%@.%@", prefix, uuidStr, ext]];
            assert(result != nil);

            CFRelease(uuidStr);
            CFRelease(uuid);

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        void iOSAlert( UIViewController * view, NSString * title, NSString * message )
        {
            UIAlertController *alertController = [UIAlertController alertControllerWithTitle:title
                                                                                     message:message
                                                                              preferredStyle:UIAlertControllerStyleAlert];

            UIAlertAction * okAction = [UIAlertAction actionWithTitle:@"OK"
                                                               style:UIAlertActionStyleDefault
                                                             handler:nil];

            [alertController addAction:okAction];

            [view presentViewController:alertController animated:YES completion:nil];
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
